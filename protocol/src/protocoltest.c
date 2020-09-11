/** @file protocoltest.c
* @brief Minimal template for using MDDriver with a fake calculation code
* @author Marc Baaden <baaden@smplinux.de>
* @author Olivier Delalande
* @author Nicolas Ferey
* @author Gilles Grasseau
* @date   2007-8
*
* Copyright Centre National de la Recherche Scientifique (CNRS)
*/
/* contributors :
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
*
* References :
* If you use this code, could you please cite one of these references :
* O. Delalande, N. Ferey, G. Grasseau and M. Baaden : "Complex Molecular Assemblies at hand via Interactive Simulations", 2009, Journal of Computational Chemistry 2009.
* N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "A VR framework for interacting with molecular simulations", 2008, in proceedings of ACM symposium on Virtual reality software and technology (ACM - VRST'08).
*
* $Id: protocoltest.c,v 1.1 2008-06-25 12:16:49 mdd Exp $
*
* A minimal template that shows how to implement calls to MDDriver from
* an example program
*/

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "imd_interface.h"

#define N 6
#define NDIM 3

float coords[N][NDIM] =
{	// Example coordinates for methanol
	{ -0.748,  -0.015,   0.024}, // 6 atoms, use methanol.pdb for
	{	0.558,   0.420,  -0.278},  // visualization in VMD
	{	-1.293,  -0.202,  -0.901},
	{	-1.263,   0.754,   0.600},
	{	-0.699,  -0.934,   0.609},
	{	0.716,   1.404,   0.137}
};

// my_imd configuration
static int   MYIMDdebug  = 0;
static FILE *MYIMDlog;
static       IMDEnergies energies; // Communication buffer for energies

int nstximd              = 0;
int myimd_wait           = 1;
int myimd_port           = -3000; // If the value is negative, we will print debug output!

void myimd_init( )
{	// --- MDD Initialization function ---
	static int fp_comm = -1;
	if ( fp_comm == -1)
	{
		MYIMDlog = stderr;
		//MYIMDdebug = IIMD_init(myimd_wait, myimd_port, MYIMDdebug, MYIMDlog, &nstximd, &MYIMDstop);
		IIMD_treatevent();
		fp_comm = 1;
	}
}

void myimd_send_traj(int* n, float* coords)
{	// --- Send our trajectory data via MDD ---
	myimd_init();
	IIMD_treatevent();
	IIMD_send_coords(n, coords);
}

void myimd_send_energies(int step_)
{	// --- Send some energy information via MDD ---
	energies.tstep  = step_;
	energies.T = energies.Etot = energies.Epot = energies.Evdw = energies.Eelec = 0;
	energies.Ebond = energies.Eangle = energies.Edihe = energies.Eimpr  = 0;
	IIMD_send_energies( &energies );
}

void myimd_ext_forces( )
{	// --- Receive some information about user applied forces via MDD ---
	int i, index;
	imd_int32 n_atoms     = 0;
	imd_int32 *atom_list  = 0;
	float     *force_list = 0;

	myimd_init();
	IIMD_treatevent();
	IIMD_get_forces( &n_atoms, &atom_list, &force_list );

	if (n_atoms != 0)
	{	// print forces if received, so we see how to use log and debug
		for (i = 0; i <  n_atoms ; i++)
		{
			index = atom_list[i];
			if ( MYIMDdebug )
			{
				fprintf(MYIMDlog, "F(%7d) %8.2e %8.2e %8.2e \n",
				        index, force_list[i * 3], force_list[i * 3 + 1], force_list[i * 3 + 2]
				       );
			}
		}
	}
}

void my_calculation(float coords[][NDIM])
{	// --- our "calculation" is just a shift in y direction ---
	int i = 0;
	for (i = 0; i < N; i++) coords[i][1] += 0.05;
#if defined(_WIN32)
	Sleep(1000);
#else
	sleep(1);
#endif
}

int main()                                                      // -- THIS IS THE MAIN PROGRAM LOOP ---
{
	int n = N;
	int i = 0;
	for (i = 0;; i++)
	{
		my_calculation(coords);
		myimd_send_traj(&n, *coords);    // send our coordinates
		myimd_send_energies(i);          // send our energies
		myimd_ext_forces();              // receive VMD's forces
		// implement condition to exit this loop end finish execution
	}
	IIMD_terminate();
	return 0;
}
