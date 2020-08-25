/**@file servertest.c
* @brief Template for using MDDriver with a simple calculation code
* @author Marc Baaden <baaden@smplinux.de>
* @author Olivier Delalande
* @author Nicolas Ferey
* @author Gilles Grasseau
* @date   2007-8
*
* Copyright Centre National de la Recherche Scientifique (CNRS)
*
* contributors :
* Marc Baaden, 2007-8
* Olivier Delalande, 2008
* Nicolas Ferey, 2008
* Gilles Grasseau, 2007-8
*
* baaden@smplinux.de
* http://www.baaden.ibpc.fr
*
* This software is a computer program whose purpose is to provide a general
* interface for coupling calculation and visualization modules via the IMD
* protocol. The part of this source code dealing directly with the IMD
* protocol was adapted from the VMD/NAMD packages.
*
* This source code is part of MDDriver, a general purpose library for
* interactive simulations. This software is governed by the CeCILL-C license
* under French law and abiding by the rules of distribution of free software.
* You can use, modify and/or redistribute the software under the terms of the
* CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL
* “http://www.cecill.info”.
*
* As a counterpart to the access to the source code and rights to copy,
* modify and redistribute granted by the license, users are provided only
* with a limited warranty and the software’s author, the holder of the
* economic rights, and the successive licensors have only limited
* liability.
*
* In this respect, the user’s attention is drawn to the risks associated
* with loading, using, modifying and/or developing or reproducing the
* software by the user in light of its specific status of free software,
* that may mean that it is complicated to manipulate, and that also
* therefore means that it is reserved for developers and experienced
* professionals having in-depth computer knowledge. Users are therefore
* encouraged to load and test the software’s suitability as regards their
* requirements in conditions enabling the security of their systems and/or
* data to be ensured and, more generally, to use and operate it in the
* same conditions as regards security.
*
* The fact that you are presently reading this means that you have had
* knowledge of the CeCILL-C license and that you accept its terms.
*
* $Id: servertest.c,v 1.2 2008-06-30 06:56:24 mdd Exp $
*
* A simple template that shows how to implement calls to MDDriver from
* an example program
*
*
* References :
* If you use this code, could you please cite one of these references :
* O. Delalande, N. Ferey, G. Grasseau and M. Baaden : "Complex Molecular Assemblies at hand via Interactive Simulations", 2009, Journal of Computational Chemistry 2009.
* N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "A VR framework for interacting with molecular simulations", 2008, in proceedings of ACM symposium on Virtual reality software and technology (ACM - VRST'08).
* N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "From Interactive to Immersive Molecular Dynamics", in Proceedings of the international Workshop on Virtual Reality and Physical Simulation (Eurographics - VRIPHYS'08).
*

*/

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "imd_interface.h"

#define N 104
#define NDIM 3

// DEFINE "CALCULATION"
// Currently availabe "calculations" for testing and debugging are:
//
//   0   just a simple displacement of the molecule
//   1   randomly jiggle atoms around their initial positions
//
#define CURRCALC 1

// Example coordinates for deca-alanine, 104 atoms, use test.pdb for visualization in VMD
float coords[N][NDIM] =
{
	{ 0.675,  -1.852,   0.841},
	{-0.060,  -1.319,   1.348},
	{1.601,  -1.923,   1.308},
	{0.767,  -1.116,  -0.506},
	{0.758,  -0.050,  -0.331},
	{2.017,  -1.635,  -1.229},
	{2.022,  -1.318,  -2.294},
	{3.029,  -1.308,  -0.907},
	{  2.073,  -2.745,  -1.245},
	{ -0.492,  -1.415,  -1.309},
	{ -1.603,  -0.788,  -1.043},
	{ -0.415,  -2.269,  -2.327},
	{  0.358,  -2.889,  -2.433},
	{ -1.508,  -2.684,  -3.195},
	{ -2.235,  -1.891,  -3.278},
	{ -1.027,  -2.948,  -4.690},
	{ -1.877,  -3.427,  -5.221},
	{ -0.815,  -2.042,  -5.297},
	{ -0.159,  -3.640,  -4.647},
	{ -2.103,  -4.015,  -2.674},
	{ -1.898,  -5.118,  -3.175},
	{ -3.008,  -3.958,  -1.721},
	{ -3.242,  -3.028,  -1.451},
	{ -3.587,  -5.145,  -1.159},
	{ -2.923,  -5.989,  -1.280},
	{ -3.691,  -4.902,   0.352},
	{ -4.247,  -3.993,   0.669},
	{ -4.290,  -5.795,   0.631},
	{ -2.660,  -4.938,   0.764},
	{ -4.940,  -5.348,  -1.716},
	{ -5.958,  -4.774,  -1.270},
	{ -5.006,  -6.148,  -2.832},
	{ -4.198,  -6.559,  -3.249},
	{ -6.149,  -6.655,  -3.521},
	{ -6.719,  -5.805,  -3.866},
	{ -5.688,  -7.554,  -4.760},
	{ -6.557,  -7.907,  -5.355},
	{ -5.078,  -6.973,  -5.484},
	{ -5.144,  -8.486,  -4.498},
	{ -7.019,  -7.516,  -2.558},
	{ -7.060,  -8.717,  -2.654},
	{ -7.741,  -6.935,  -1.682},
	{ -7.636,  -5.946,  -1.624},
	{ -8.703,  -7.597,  -0.818},
	{ -8.260,  -8.519,  -0.470},
	{ -8.930,  -6.711,   0.398},
	{ -9.546,  -5.809,   0.192},
	{ -9.457,  -7.317,   1.165},
	{ -7.967,  -6.413,   0.866},
	{-10.071,  -7.991,  -1.463},
	{-11.097,  -7.467,  -1.130},
	{-10.118,  -9.060,  -2.375},
	{ -9.200,  -9.438,  -2.464},
	{-11.309,  -9.766,  -2.682},
	{-12.088,  -9.050,  -2.900},
	{-11.096, -10.447,  -4.029},
	{-10.689, -11.477,  -3.933},
	{-12.020, -10.599,  -4.626},
	{-10.532,  -9.820,  -4.752},
	{-11.734, -10.620,  -1.476},
	{-11.625, -11.841,  -1.447},
	{-12.408, -10.053,  -0.445},
	{-12.464,  -9.064,  -0.333},
	{-12.813, -10.853,   0.708},
	{-12.033, -11.549,   0.979},
	{-13.094,  -9.797,   1.816},
	{-13.830,  -8.998,   1.584},
	{-13.523, -10.308,   2.705},
	{-12.221,  -9.141,   2.016},
	{-14.072, -11.640,   0.485},
	{-15.103, -11.354,   1.127},
	{-14.072, -12.637,  -0.437},
	{-13.239, -12.903,  -0.915},
	{-15.146, -13.518,  -0.873},
	{-16.028, -12.901,  -0.953},
	{-14.640, -14.114,  -2.143},
	{-14.193, -13.248,  -2.675},
	{-13.891, -14.902,  -1.914},
	{-15.483, -14.546,  -2.723},
	{-15.636, -14.522,   0.191},
	{-15.524, -15.772,   0.088},
	{-16.285, -14.019,   1.277},
	{-16.412, -13.032,   1.338},
	{-16.630, -14.758,   2.413},
	{-15.931, -15.577,   2.492},
	{-16.298, -14.037,   3.670},
	{-16.760, -14.659,   4.466},
	{-15.190, -14.029,   3.752},
	{-16.567, -12.967,   3.798},
	{-18.091, -15.164,   2.368},
	{-18.913, -14.589,   1.724},
	{-21.002, -16.274,   3.801},
	{-21.221, -16.919,   4.815},
	{-21.819, -15.255,   3.410},
	{-22.632, -15.022,   3.945},
	{-21.519, -14.695,   2.639},
	{-18.426, -16.340,   2.929},
	{-17.745, -16.940,   3.341},
	{-19.913, -16.805,   2.813},
	{-20.249, -16.535,   1.822},
	{-19.961, -18.362,   2.757},
	{-20.844, -18.848,   2.290},
	{-19.088, -18.784,   2.215},
	{-19.817, -18.835,   3.753}
};


// my_imd configuration
static int   MYIMDdebug  = 1;
static FILE *MYIMDlog = NULL;
static int   MYIMDstop   = 0;
static int   MYIMDpause = 0;
static int mode = 1; //server
// Converting Calories x Angstrom-1
// to Joules x nm-1
const double CaloryToJoule    = 4.184;
const double CaloryAToJouleNM = 4.184*10.0;
const double JouleToCalory    = 1. / 4.184;
const double NmToAngstrom     = 10.0;

// Communication buffer for energies
static IMDEnergies energies;

int nstximd              = 0;
int myimd_wait           = 1;
int myimd_log           = -1;

// If the value is negative, we will print debug output!
int myimd_port           = 3000;


// MDD Initialization function
void myimd_init( )
	{
	// IMD initialisation
	if (!IIMD_probeconnection())
		{
		MYIMDlog= IIMD_init( "",&mode, &myimd_wait, &myimd_port, &myimd_log,0 );
		#if defined(_WIN32)
			Sleep(500);
		#else
			usleep(500000);
		#endif
		}
	}


// -----------------------------------------------------------
// BELOW COME THE MAIN ADAPTER FUNCTIONS BETWEEN THE MDDRIVER
// CORE AND THE CALCULATION CODE
// -----------------------------------------------------------

// Send our trajectory data via MDD; this should only be done on the master process
// if we are in a parallel calculation module (MPI and such)
void myimd_send_traj(int* n, float* coords)
	{
	if ( MYIMDdebug )
		{
		fprintf(MYIMDlog, "MYMDD > \n");
		fprintf(MYIMDlog, "MYMDD > Sending %d atom positions \n", *n);
		fprintf(MYIMDlog, "MYMDD > \n");
		}

	// Let's send our coordinates to the visualization program
	IIMD_send_coords(n, coords);
	// More complex routines like for gromacs/gmx_send_traj can do...
	//..conversion from nm to Angstrom
	//..other data structure transformations
	}


// Send some energy information via MDD; this should only be done on the master process
// if we are in a parallel calculation module (MPI and such)
void myimd_send_energies(int step_)
	{
	energies.tstep  = step_;
	// normally we would retrieve the values below from some data structure in
	// the program, here we just assign some random values
	energies.T      = 300.0 + 10.0 * ( (double)rand() / (double)(RAND_MAX) );
	energies.Etot   = 999.0 + 40.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;
	energies.Epot   = 555.0 + 30.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;
	energies.Evdw   = 111.0 + 20.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;
	energies.Eelec  = 333.0 + 30.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;
	energies.Ebond  =  33.0 + 10.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;
	energies.Eangle =  33.0 + 10.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;
	energies.Edihe  =  33.0 + 10.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;
	energies.Eimpr  =  33.0 + 10.0 * ( (double)rand() / (double)(RAND_MAX) ) * JouleToCalory;

	// should we print the energies to the log file?
	if ( MYIMDdebug )
		{

		fprintf(MYIMDlog, "MYMDD > \n");
		fprintf(MYIMDlog, "MYMDD > Send energies (Time step=%d)\n", step_);
		fprintf(MYIMDlog, "MYMDD > ================================\n");
		fprintf(MYIMDlog, "MYMDD >   \n");
		fprintf(MYIMDlog, "MYMDD >   MYPROGRAM Energy List (%d) \n", 99 );
		fprintf(MYIMDlog,
		"MYMDD >   [Cal] for energies, [K] for the temperature \n" );
		fprintf(MYIMDlog,
		"MYMDD >   [Bar] for pressure\n" );
		fprintf(MYIMDlog,
		"MYMDD >   ------------------------------------------ \n");
		fprintf(MYIMDlog, "MYMDD >  \n");
		fprintf(MYIMDlog, "MYMDD >   VMD Energy List \n" );
		fprintf(MYIMDlog, "MYMDD >   --------------------\n");
		fprintf(MYIMDlog, "MYMDD >   Time step         [ ]   %12d\n",
		energies.tstep);
		fprintf(MYIMDlog, "MYMDD >   Temperature       [K]   %12.5e\n",
		energies.T);
		fprintf(MYIMDlog, "MYMDD >   Total E.          [Cal] %12.5e\n",
		energies.Etot);
		fprintf(MYIMDlog, "MYMDD >   Bond E.           [Cal] %12.5e\n",
		energies.Ebond);
		fprintf(MYIMDlog, "MYMDD >   Angle E.          [Cal] %12.5e\n",
		energies.Eangle);
		fprintf(MYIMDlog, "MYMDD >   Potential E.      [Cal] %12.5e\n",
		energies.Epot);
		fprintf(MYIMDlog, "MYMDD >   Dihedrale E.      [Cal] %12.5e\n",
		energies.Edihe);
		fprintf(MYIMDlog, "MYMDD >   Improp. Dihed. E. [Cal] %12.5e\n",
		energies.Eimpr);
		fprintf(MYIMDlog, "MYMDD >   Van der Waals E.  [Cal] %12.5e\n",
		energies.Evdw);
		fprintf(MYIMDlog, "MYMDD >   Electrostatic. E. [Cal] %12.5e\n",
		energies.Eelec);
		fprintf(MYIMDlog, "MYMDD > \n");
		}

	// SEND ENERGIES
	IIMD_send_energies( &energies );
	// More complex routines like for gromacs/MYIMD_send_energies can do...
	// ..whatever you want, for example unit conversion
	}


// Receive some information about user applied forces via MDD; this should only be done
// on the master process if we are in a parallel calculation module (MPI and such)
void myimd_ext_forces( )
	{
	int i;
	// Broadcast Number of forces, Atoms list and force list
	imd_int32 n_atoms     = 0;
	imd_int32 *atom_list  = 0;
	float     *force_list = 0;
	if ( MYIMDdebug )
		{
		fprintf(MYIMDlog,"MYMDD > \n");
		fprintf(MYIMDlog,"MYMDD >   Forces \n");
		fprintf(MYIMDlog,"MYMDD > ==========\n");
		fprintf(MYIMDlog,"MYMDD > \n");
		}


	IIMD_get_forces( &n_atoms, &atom_list, &force_list );

	if ( MYIMDdebug )
		{
		fprintf(MYIMDlog,"MYMDD > \n");
		fprintf(MYIMDlog,
		"MYMDD > Number of Ext. forces dF = %d\n", n_atoms);
		fprintf(MYIMDlog,"MYMDD  > \n");
		if( n_atoms != 0 )
			{
			fprintf(MYIMDlog,"MYMDD >  Force list [J/nm]\n");
			fprintf(MYIMDlog,
			"MYMDD > Proc  AtomID     Fx       Fy       Fz      dFx      dFy      dFz    \n");
			fprintf(MYIMDlog,
			"MYMDD > -------------------------------------------------------------------------\n");
			}
		}

	if (n_atoms != 0)
		{
		int index;

		for (i=0; i <  n_atoms ; i++)
			{
			index = atom_list[i];
			if ( MYIMDdebug )
				{
				fprintf(MYIMDlog,"MYMDD > %4d %7d %8.2e %8.2e %8.2e %8.2e %8.2e %8.2e \n",
				1, index,
				//CaloryToJoule *
				force_list[i*3],
				//CaloryToJoule *
				force_list[i*3+1],
				//CaloryToJoule *
				force_list[i*3+2],0.0,0.0,0.0
				);
				}
			// Do whatever you want with the forces from force_list!
			// for example update a per-atom force array..
			// ..note that VMD sends forces in calories, so here we convert to
			// Joule for example. Depends on your software.
			}
		if ( MYIMDdebug )
			{
			fprintf(MYIMDlog,"MYMDD > \n");
			}
		}
	}


// -----------------------------------------------------------
// THE TWO FUNCTIONS BELOW ARE FOR OUR FAKE CALCULATION MODULE
// -----------------------------------------------------------

// HERE WE INITIALIZE OUR COORDINATES WITH THE REFERENCE DATA
void init_coords(float coord_out[][NDIM])
	{
	int i=0;
	for (i=0; i< N; i++)
		{
		coord_out[i][0] = coords[i][0];
		coord_out[i][1] = coords[i][1];
		coord_out[i][2] = coords[i][2];
		}
	}

// HERE WE "CALCULATE"
void my_calculation(float coord_out[][NDIM])
	{
	float	tmp;
	int i=0;
	for (i=0; i< N; i++)
		{
		// Just displace the molecule
		if (CURRCALC == 0 )
			{
			coord_out[i][1] = coord_out[i][1] + 0.05;

			// jitter atom coordinate until the connection is terminated //
			}
		else if (CURRCALC == 1 )
			{
			//  generate random coordinates for this test
			tmp = 1.0 * ( (double)rand() / (double)(RAND_MAX) );
			//	if ( MYIMDdebug )
			//		fprintf(MYIMDlog, "MYMDD > %8.5f \n", tmp);
			coord_out[i][0] = coords[i][0] + tmp ;  /* coordinate x */
			coord_out[i][1] = coords[i][1] - tmp ;  /* coordinate y */
			coord_out[i][2] = coords[i][2] + tmp;   /* coordinate z */
			}
		}

	// Control the "speed" of the calculation
	#if defined(_WIN32)
		Sleep(50);
	#else
		usleep(50000);
	#endif
	}


// -----------------------------------------------------------
// BELOW COMES THE MAIN PROGRAM LOOP WITH CALLS TO MDDRIVER
// -----------------------------------------------------------

int main()
	{
	int       n=N;
	float     tmpcoords[N][NDIM];
	int i=0;
	// initialize the coordinate array
	init_coords(tmpcoords);

	myimd_init();

	for(i=0; MYIMDstop == 0; i++)
		{
		IIMD_probeconnection();
		IIMD_treatprotocol();

		if(!MYIMDpause)
			{
			// Calculate and update our coordinates
			my_calculation(tmpcoords);

			// DO THE "IMD"
			myimd_send_traj(&n, *tmpcoords); // send our coordinates
			myimd_send_energies(i);          // send our energies
			myimd_ext_forces();              // receive VMD's forces
			}
		// Treats extra events
		switch( imd_event )
			{
			if(imd_event==IMD_PAUSE)
			 		exit(0);
			case IMD_KILL:
			MYIMDstop = 1;
			fprintf(MYIMDlog,"MYMDD > Kill received\n");
			// Important : clear the event
			imd_event = -1;
			break;
			case IMD_TRATE:
			fprintf(MYIMDlog,"MYMDD > Trate received\n");
			nstximd = imd_value;
			// Important : clear the event
			imd_event = -1;
			break;
			case IMD_PAUSE:
					fprintf(MYIMDlog,"MYMDD > Pause received\n");
					if(MYIMDpause==1)
						{
						MYIMDpause = 0;
						}
					else
						{
						MYIMDpause = 1;
						}
			imd_event = -1;

			break;
			}
		#if defined(_WIN32)
			Sleep(500);
		#else
			usleep(500000);
		#endif
		}

	// now finish gracefully
	fprintf(MYIMDlog,"MYMDD > \n");
	fprintf(MYIMDlog,"MYMDD > stop simulation\n");
	IIMD_terminate();

	return 0;
	}
