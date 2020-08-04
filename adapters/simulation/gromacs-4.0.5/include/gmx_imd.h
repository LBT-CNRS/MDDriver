//=======================================================
//
// Interface with Gromacs
// IDRIS (July 2007)
// 
//   Interfacing routines between Gromacs (mdrun) and IMD 
//   Start with "gimd_" prefix
//
//=======================================================

#ifndef GMX_IMD_H__
#define GMX_IMD_H__

#include "typedefs.h"
#include "ebin.h"

// Stored in gmx_imd.c
extern int nstximd;
extern int gimd_wait;
extern int gimd_port;
extern int gimd_msg;
extern char * gimd_log;

void gimd_init( );

void gimd_send_traj( t_commrec *cr, t_mdatoms *mdatoms,
		     int step, real t,
		     int natoms, rvec *xx, rvec *vv, rvec *ff,
		     matrix box); 

void gimd_send_energies( t_commrec *cr, int step_, t_ebin *en_, int index , 
			 int n_en_);

void gimd_ext_forces( int step_, t_commrec *cr, 
		      t_mdatoms *mdatoms, rvec *force );
 
void gimd_treat_filtered_event( );

int  gimd_stop_simulation( ) ;

void gimd_terminate( t_commrec *cr ) ;


#endif
