//=======================================================
//
// Interface with Gromacs
// IDRIS (July 2007)
// 
//   Interfacing routines between Gromacs (mdrun) and IMD 
//   Start with "gimd_" prefix
//
//=======================================================

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "imd_interface.h"
#include "gmx_imd.h"
#include "vec.h"
#include "partdec.h"
#include "mvdata.h"
#include "smalloc.h"

#if ( !defined(GMX_MPI))

typedef int Int;  // Must be 4 bytes 

Int  MPI_INT        = 1;
Int  MPI_COMM_WORLD = 0;
  

Int MPI_Bcast( void *array, Int n, Int MPI_type_, Int root_pid , 
	       Int comm_ ) 
{ return 0; }

Int MPI_Barrier( Int comm_ ) { return 0; }

#else
#include "mpi.h"
#endif

// gmx_imd configuration
static int   GIMD_UseBroadcast = 0;
static char *GIMDfilename      = "mddriver.log";
static FILE *GIMDlog;

// Converting Calories x Angstrom-1
// to Joules x nm-1
const double CaloryToJoule    = 4.184;
const double CaloryAToJouleNM = 4.184*10.0;
const double JouleToCalory    = 1. / 4.184;
const double NmToAngstrom     = 10.0;

// Communication buffer for energies
static IMDEnergies energies;

int nstximd   = 0;
int gimd_wait = 1;
int gimd_port = 3000;
int gimd_mode = 1;//1 for server, 0 for client
int gimd_msg  = 0 ;
char * gimd_log = "mddriver.log";
static int g_event = -1;  // Use to broadcast IMD event and the IMD value
static int g_value = 0;
static int MasterPID = 0; // GMX Root or Master process identifier

static void _moveit(int left,int right,char *s,rvec xx[],t_mdatoms *mdatoms,t_commrec *cr)
{
  rvec  *temp;
  int   i,m,bP,start,homenr;

  if (!xx) 
    return;

  start=mdatoms->start;
  homenr=mdatoms->homenr;

  if ( MASTER(cr) )
  	if ( gimd_msg > 1 )
    		fprintf(GIMDlog,"GMXMDD > Sending trajectory, start=%d, homenr=%d\n",start,homenr);

  snew(temp,homenr);
  for(i=0; (i<homenr); i++)
    copy_rvec(xx[start+i],temp[i]);

  // Changed : move_rvecs(cr,FALSE,FALSE,left,right,xx,NULL,pd_shift(cr),NULL);
  move_rvecs(cr,FALSE,FALSE,left,right,
  	     xx,NULL,(cr->nnodes-cr->npmenodes)-1,NULL);
  for(i=0; (i<homenr); i++) {
    bP=0;
    for(m=0; (m<DIM); m++)
      if (xx[start+i][m] != temp[i][m])
	bP=1;
	if ( MASTER(cr) )
    		if (bP && gimd_msg > 1 )
		      fprintf(GIMDlog,"GMXMDD %s[%5d] before: (%8.3f,%8.3f,%8.3f)"
	      		" After: (%8.3f,%8.3f,%8.3f)\n",
	      			s,start+i,temp[i][XX],temp[i][YY],temp[i][ZZ],
	      xx[start+i][XX],xx[start+i][YY],xx[start+i][ZZ]);
  }
  sfree(temp);
}

#define MX(xvf) _moveit(GMX_LEFT,GMX_RIGHT,#xvf,xvf,mdatoms,cr)

void gimd_init( ) {
static int fp_comm = -1;

  // IMD initialisation
  if ( fp_comm == -1) {
    GIMDlog = stderr;
    GIMDlog = IIMD_init( "", &gimd_mode, &gimd_wait, &gimd_port, &gimd_msg, gimd_log ); 
    IIMD_probeconnection();
    IIMD_treatprotocol();
    fp_comm = 1;
  }
}

void gimd_send_traj( t_commrec *cr, t_mdatoms *mdatoms,
		     int step, real t,
		     int natoms, rvec *xx, rvec *vv, rvec *ff,
		     matrix box) {
  int i, j;
  

  if ( MASTER(cr) ) {
     gimd_init( );

     IIMD_probeconnection();
     IIMD_treatprotocol();
  }

  // Collecte coordinates (if the Nbr Processors > 1)
  if (cr->nnodes > 1) {
    MX(xx);
    // Not implemented
    // MX(vv);
    // MX(ff);
    vv = 0; ff = 0;
  }

  if MASTER(cr){
    if ( xx ) {
      rvec *coord = (rvec *) malloc( sizeof(rvec) * natoms );
      // Normalisation
      for (i = 0; i < natoms; i++) {
	for (j = 0; j < 3; j++) {
	  coord[i][j] = xx[i][j] * NmToAngstrom;
      }}
      if ( gimd_msg > 1 ) {
        fprintf(GIMDlog, 
	    "GMXMDD > \n");
	fprintf(GIMDlog, "GMXMDD > Send %d atoms (Time step=%d)\n", 
		natoms, step);
	fprintf(GIMDlog, "GMXMDD > ================================\n");
        fprintf(GIMDlog, 
	    "GMXMDD > \n");
      }

      IIMD_send_coords( &natoms, (float *) (coord) );
      free(coord); 
      // Not implemented
      // iimd_send_velocities( natoms, vv);
      // iimd_send_forces    ( natoms, ff);
    }
  }
}

// Broadcast an array of 4-bytes elements
void gimd_bcast_4bytes(  int root_pid, void *array , int n){

  int rc = MPI_Bcast( array, n, MPI_INT, root_pid , MPI_COMM_WORLD );
}

void gimd_barrier( ){

  int rc = MPI_Barrier( MPI_COMM_WORLD );
}


void gimd_send_energies( t_commrec *cr, int step_, t_ebin *en_, 
			 int index , int n_en_)
{
  int i, j;
  
  static clock_t last_clock = 0;
  static clock_t cur_clock;
  static int last_step = - INT_MIN + 1;


  if ( MASTER(cr) ) {
    cur_clock = clock();
 
    energies.tstep  = step_;
    energies.T      = en_->e[index+ 10].e; 
    energies.Etot   = en_->e[index+ 9].e * JouleToCalory ;
    energies.Epot   = en_->e[index+ 7].e * JouleToCalory ; 
    // LJ-14 + LJ (SR) - > (3) + (5)
    energies.Evdw   = (en_->e[index+3].e + en_->e[index+ 5].e )* JouleToCalory;
    // Coulomb-14 + Coulomb (SR) - > (4) + (6)
    energies.Eelec  = (en_->e[index+4].e + en_->e[index+ 6].e) * JouleToCalory;
    // Replaced by the rate iteretions / seconds
    // energies.Ebond  = 0.0;
    energies.Ebond  = (double)((step_ - last_step) * CLOCKS_PER_SEC )
                    / (cur_clock -  last_clock);
    if (energies.Ebond < 0.0)
      energies.Ebond = 0.0;

    // Case base clock resolution
    if ( cur_clock ==  last_clock )
      energies.Ebond = -1.0;
    
    energies.Eangle = en_->e[index+ 0].e * JouleToCalory;
    energies.Edihe  = en_->e[index+ 1].e * JouleToCalory;
    energies.Eimpr  = en_->e[index+ 2].e * JouleToCalory;

    if ( gimd_msg > 1 ) {
      fprintf(GIMDlog, "GMXMDD > \n");
      fprintf(GIMDlog, "GMXMDD > Send energies (Time step=%d)\n", step_);
      fprintf(GIMDlog, "GMXMDD > ================================\n");
      fprintf(GIMDlog, "GMXMDD >   \n");
      fprintf(GIMDlog, "GMXMDD >   GROMACS Energy List (%d) \n", n_en_ );
      fprintf(GIMDlog, 
	      "GMXMDD >   [Cal] for energies, [K] for the temperature \n" );
      fprintf(GIMDlog, 
	      "GMXMDD >   [Bar] for pressure\n" );
      fprintf(GIMDlog, 
	      "GMXMDD >   ------------------------------------------ \n");

      for (i=0; i< n_en_; i++){
	if ( i == 10 ) {
	  fprintf(GIMDlog, "GMXMDD >   (%15s) = %12.5e \n", 
              en_->enm[index+i], en_->e[index+ i].e);  
	}
	else {
	  fprintf(GIMDlog, "GMXMDD >   (%15s) = %12.5e \n", 
              en_->enm[index+i], en_->e[index+ i].e * JouleToCalory);  
	}
      }

      fprintf(GIMDlog, "GMXMDD >  \n");
      fprintf(GIMDlog, "GMXMDD >   VMD Energy List \n" );
      fprintf(GIMDlog, "GMXMDD >   --------------------\n");
      fprintf(GIMDlog, "GMXMDD >   Time step         [ ]     %12d\n", 
	      energies.tstep);
      fprintf(GIMDlog, "GMXMDD >   Temperature       [K]     %12.5e\n", 
	      energies.T);
      fprintf(GIMDlog, "GMXMDD >   Total E.          [Cal]   %12.5e\n", 
	      energies.Etot);
      fprintf(GIMDlog, "GMXMDD >   Rate (Bond E.)    [n/sec] %12.5e\n", 
	      energies.Ebond);
      fprintf(GIMDlog, "GMXMDD >   Angle E.          [Cal]   %12.5e\n", 
	      energies.Eangle);
      fprintf(GIMDlog, "GMXMDD >   Potential E.      [Cal]   %12.5e\n", 
	      energies.Epot);
      fprintf(GIMDlog, "GMXMDD >   Dihedrale E.      [Cal]   %12.5e\n", 
	      energies.Edihe);
      fprintf(GIMDlog, "GMXMDD >   Improp. Dihed. E. [Cal]   %12.5e\n", 
	      energies.Eimpr);
      fprintf(GIMDlog, "GMXMDD >   Van der Waals E.  [Cal]   %12.5e\n", 
	      energies.Evdw);
      fprintf(GIMDlog, "GMXMDD >   Electrostatic. E. [Cal]   %12.5e\n", 
	      energies.Eelec);
      fprintf(GIMDlog, "GMXMDD > \n");
    }
    IIMD_send_energies( &energies );

    last_clock =  cur_clock;
    last_step  = step_;
  }

  gimd_treat_filtered_event( );
}

// Processing filtered events coming from IMD 
// Must be done by all processors.
// To minimize edge effects it is better
// to run this function at the end of the 
// main timedependent loop
// For Gromacs case this is done in the function 
// which sends the energies.
//
void gimd_treat_filtered_event( )
{
  // Broadcast the event
  imd_int64 array[2];
  array[0] = imd_event;
  array[1] = imd_value;
  gimd_bcast_4bytes( MasterPID, array, (int) (4) );
  g_event = (imd_int32) array[0];
  g_value = array[1];

  if (g_event >= 0) {
    switch(g_event){
    case IMD_TRATE:
      nstximd   = g_value;
      // Clear the event
      g_event   = -1;
      imd_event = -1;
      break;
    case IMD_KILL:
      // Terminal event
      // Do not clear the event
      break;
    }
  }
}

void gimd_ext_forces( int step_, t_commrec *cr, t_mdatoms *mdatoms, rvec *forces ) {
  int i;
  
  // The master process deal with the connection 
  if MASTER(cr){

    gimd_init( );

    if ( gimd_msg > 0 ) {
      fprintf(GIMDlog,"GMXMDD > \n");
      fprintf(GIMDlog,"GMXMDD >   Forces (Time step=%d) \n",  step_ );
      fprintf(GIMDlog,"GMXMDD > =============================\n");
      fprintf(GIMDlog,"GMXMDD > \n");
    }

    // Probe connexion
    IIMD_probeconnection();
    IIMD_treatprotocol();

  }

  // Broadcast Number of forces, Atoms list and force list
  imd_int32 n_atoms = 0;
  imd_int32 *atom_list  = 0;
  float     *force_list = 0;

  if MASTER(cr)
    IIMD_get_forces( &n_atoms, &atom_list, &force_list ); 

  if (  MASTER(cr) && gimd_msg > 0 ) {
    fprintf(GIMDlog,"GMXMDD > \n");

    fprintf(GIMDlog,
	    "GMXMDD > Number of Ext. forces dF = %d\n", n_atoms);
    fprintf(GIMDlog,"GMXMDD > \n");
    if( n_atoms != 0 ) {
      fprintf(GIMDlog,"GMXMDD >  Force list [J/nm]\n");
      fprintf(GIMDlog,
"GMXMDD > Proc  AtomID     Fx       Fy       Fz      dFx      dFy      dFz    \n");
      fprintf(GIMDlog,
"GMXMDD > -------------------------------------------------------------------------\n");
    }
  }


  if ( GIMD_UseBroadcast ) {

    gimd_bcast_4bytes( MasterPID, &n_atoms, (int) (1) );

    if ( n_atoms != 0 ) {

      if ( cr -> nodeid != MasterPID ) {
	atom_list  = (imd_int32 *) malloc( sizeof(imd_int32) * n_atoms     );
	force_list = (float *)     malloc( sizeof(float)     * n_atoms * 3 );
      } 
      gimd_bcast_4bytes( MasterPID, atom_list, n_atoms );
      gimd_bcast_4bytes( MasterPID, force_list, n_atoms*3 );

      int s_index  = mdatoms->start;
      int e_index  = s_index + mdatoms->homenr;
      int index;

      if ( (GIMDlog == stderr) && (gimd_msg > 0) ) {
	fprintf(GIMDlog, "GMXMDD > \n");
	fprintf(GIMDlog, "GMXMDD > Using Broadcast \n");
	fprintf(GIMDlog, "GMXMDD > \n");
        int ip;
	for ( ip == 0; ip < cr->npmenodes ; ip++) {
	  if ( ip == cr -> nodeid ) {
	    for (i=0; i <  n_atoms ; i++) {
	      index = atom_list[i];
	      if ( (index >= s_index ) && ( index < e_index ) ) {
		fprintf(GIMDlog,
		   "GMXMDD > %4d %7d %8.2e %8.2e %8.2e %8.2e %8.2e %8.2e \n", 
			cr->nodeid, index, 
			forces[index][0], CaloryAToJouleNM * force_list[i*3],
			forces[index][1], CaloryAToJouleNM * force_list[i*3+1],
			forces[index][2], CaloryAToJouleNM * force_list[i*3+2]
		  );
	      }
	    }
	  }
	  gimd_barrier();
	}
	fprintf(GIMDlog,"GMXMDD > \n");
      }

      for (i=0; i <  n_atoms ; i++) {
	index = atom_list[i];
	if ( (index >= s_index ) && ( index < e_index ) ) {
	  forces[index][0] += CaloryAToJouleNM * force_list[i*3];
	  forces[index][1] += CaloryAToJouleNM * force_list[i*3+1];
	  forces[index][2] += CaloryAToJouleNM * force_list[i*3+2];
	} 
      }

      if ( cr -> nodeid != MasterPID ) {
	free(atom_list);
	free(force_list);
      } 
    }
  }
  else {
    // 
    // Only MASTER deals with External Forces 
    //
    if ( MASTER(cr) && (n_atoms != 0) ) {
      
      int index;


      for (i=0; i <  n_atoms ; i++) {
	index = atom_list[i];
	if ( gimd_msg > 0 ) {
	  fprintf(GIMDlog,
		  "GMXMDD > %4d %7d %8.2e %8.2e %8.2e %8.2e %8.2e %8.2e \n", 
		  cr->nodeid, index, 
		  forces[index][0], CaloryAToJouleNM * force_list[i*3],
		  forces[index][1], CaloryAToJouleNM * force_list[i*3+1],
		  forces[index][2], CaloryAToJouleNM * force_list[i*3+2]
		  );
	} 

	forces[index][0] += CaloryAToJouleNM * force_list[i*3];
	forces[index][1] += CaloryAToJouleNM * force_list[i*3+1];
	forces[index][2] += CaloryAToJouleNM * force_list[i*3+2];

      }
      if ( gimd_msg > 0 ) {
	fprintf(GIMDlog,"GMXMDD > \n");
      } 
    }
  }
}

int gimd_stop_simulation( ) 
{
  int rc=0;
  if ( g_event == IMD_KILL ){
    if ( (GIMDlog == stderr) && ( gimd_msg > 0 ) ) {
      fprintf(GIMDlog,"GMXMDD > \n");
      fprintf(GIMDlog,"GMXMDD >   Stop simulation\n");
      fprintf(GIMDlog,"GMXMDD > ===================\n");
      fprintf(GIMDlog,"GMXMDD > \n");
    }
    rc = 1 ;
  }
  return rc;
}

void gimd_terminate( t_commrec *cr ) 
{
 if MASTER(cr){
   IIMD_terminate();
 }
}
