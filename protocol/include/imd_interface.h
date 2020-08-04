 /*
 * Copyright Centre National de la Recherche Scientifique (CNRS)
 * This source code is part of MDDriver, a general purpose library for
 * interactive simulations. This software is governed by the CeCILL-C license
 * under French law and abiding by the rules of distribution of free software.
 * You can use, modify and/or redistribute the software under the terms of the
 * CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL 
 * "http://www.cecill.info".
 * 
 * As a counterpart to the access to the source code and rights to copy, 
 * modify and redistribute granted by the license, users are provided only 
 * with a limited warranty and the software's author, the holder of the 
 * economic rights, and the successive licensors have only limited 
 * liability.
 *
 * In this respect, the user attention is drawn to the risks associated 
 * with loading, using, modifying and/or developing or reproducing the 
 * software by the user in light of its specific status of free software, 
 * that may mean that it is complicated to manipulate, and that also 
 * therefore means that it is reserved for developers and experienced 
 * professionals having in-depth computer knowledge. Users are therefore 
 * encouraged to load and test the software's suitability as regards their 
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and, more generally, to use and operate it in the 
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had 
 * knowledge of the CeCILL-C license and that you accept its terms.
 
 * References : 
 * If you use this code, could you please cite one of these references : 	
 * O. Delalande, N. Ferey, G. Grasseau and M. Baaden : "Complex Molecular Assemblies at hand via Interactive Simulations", 2009, Journal of Computational Chemistry 2009.
 * N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "A VR framework for interacting with molecular simulations", 2008, in proceedings of ACM symposium on Virtual reality software and technology (ACM - VRST'08).
 * N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "From Interactive to Immersive Molecular Dynamics", in Proceedings of the international Workshop on Virtual Reality and Physical Simulation (Eurographics - VRIPHYS'08).
 *
 */

/* $Id: imd_interface.h,v 1.11 2008-07-09 10:02:22 mdd Exp $*/


/** @file imd_interface.h
 * @brief Header file for MDDriver interface
 * @author Marc Baaden <baaden@smplinux.de> 
 * @author Gilles Grasseau <gilles.grasseau@idris.fr>
 * @author Olivier Delalande <olivier.delalande@ibpc.fr>
 * @author Nicolas Ferey <nicolas.ferey@ibpc.fr>
 * @author This source code was adapted from the VMD/NAMD packages.
 * @date   2009, June
 */

#ifndef IMD_INTERFACE_H__
#define IMD_INTERFACE_H__

# include "imd.h"
#include <stdio.h>

extern imd_int32 imd_event;
extern imd_int64 imd_value;

#ifdef UNDERSCORE 
	#define  IIMD_init                iimd_init_
	#define  IIMD_probeconnection     iimd_probeconnection_
	#define  IIMD_treatprotocol       iimd_treatprotocol_
	#define  IIMD_treatevent          iimd_treateven_
	#define  IIMD_send_coords         iimd_send_coords_
	#define  IIMD_send_energies       iimd_send_energies_
	#define  IIMD_send_energy_array   iimd_send_energy_array_
	#define  IIMD_send_forces         iimd_send_forces_
	#define  IIMD_get_coords          iimd_get_coords_
	#define  IIMD_get_energies        iimd_get_energies_
	#define  IIMD_get_forces          iimd_get_forces_
	#define  IIMD_terminate           iimd_terminate_
#else
	#define  IIMD_init                iimd_init
	#define  IIMD_probeconnection     iimd_probeconnection
	#define  IIMD_treatprotocol       iimd_treatprotocol
	#define  IIMD_treatevent          iimd_treateven
	#define  IIMD_send_coords         iimd_send_coords
	#define  IIMD_send_energies       iimd_send_energies
	#define  IIMD_send_energy_array   iimd_send_energy_array
	#define  IIMD_send_forces         iimd_send_forces
	#define  IIMD_get_coords          iimd_get_coords
	#define  IIMD_get_energies        iimd_get_energies
	#define  IIMD_get_forces          iimd_get_forces
	#define  IIMD_terminate           iimd_terminate
#endif

#ifdef __cplusplus
	extern "C" 
		{
#endif

		/**
		Must be call before by the server (simulation code) and by the client
		before starting a interactive molecular dynamics.
		@param hostname hostname providing coordinates or forces 
		@param mode set to 0 if this is the client, 1 if server
		@param IMDwait_ set to 1 to wait for the client connexion before starting molecular simulation, 0 else
		@param IMDport_ port to listen or send
		@param IMDmsg_ level of message logging: from 0 for no message to 3 for all messages.
		@return file address of log file
		*/
		extern FILE *IIMD_init(const char*  hostname, imd_int32   *mode, imd_int32   *IMDwait_,  imd_int32   *IMDport_,  imd_int32   *IMDmsg_, const char*  IMDfilename_ );
		/**
		Check for incoming connection.
		@return if connection failad return 1 else return 0
		*/
		extern int   IIMD_probeconnection   ( );

		/**
		Check/get new events from molecular simulation
		*/
		extern void  IIMD_treatprotocol     ( );

		extern void IIMD_treatevent( );

		/**
		Send all atoms coordinates on network
		@param N number of atoms of the molecular system
		@param coords x y z coordinates arrays of all the atoms (the array has n_atoms*3 float) 
		*/
		extern void  IIMD_send_coords       ( const int *N, const float* coords );


		/**
		Send molecular system energies on network
		@param energies energiesof the molecular system
		*/
		extern void  IIMD_send_energies     ( const IMDEnergies *energies );

		/**
		Send molecular system energies computed at each timestep on network
		@param tstep energies of the molecular system
		@param energies array of energies of the molecular system at each timestep
		*/
		extern void  IIMD_send_energy_array ( const imd_int32   *tstep, 
						      const IMDEnergies *energies 
						    );

		/**
		Send forces apply on the atoms of the molecular system
		@param n_atoms number of atoms on which a force is applying
		@param AtomIndex atom id arrays on which a force is applying(the array have n_atoms integers)
		@param forces x y z force components applying on each atom in the atom_list id array (this array must have n_atoms*3 floats)
		*/
		extern void  IIMD_send_forces       ( const int *N_p, const int* AtomIndex, const float *forces );

		/**
		Disconnect the MDDriver connection
		*/
		extern void  IIMD_send_disconnect   ( );

		/**
		Pause the MDDriver connection
		*/
		extern void  IIMD_send_pause        ( );

		/**
		Kill the MDDriver connection
		*/
		extern void  IIMD_send_kill         ( );

		/**
		Change the number of timesteps between sending results
		@param trate number of timesteps between sending results
		*/
		extern void  IIMD_send_trate        ( imd_int32 *trate);

		/**
		Get atom coordinates x y z of the molecular system
		@param n_atoms number of atom in the molecular system
		@param x y z coordinates arras of all the atoms (the array has n_atoms*3 float)
		@return file address of log file
		*/
		extern int   IIMD_get_coords        ( imd_int32 *n_atoms, float **coords);

		/**
		Get energies of the molecular system
		@param energ_ energy of the system
		@return id energies are avalaible return 1 else return 0
		*/
		extern int   IIMD_get_energies      ( IMDEnergies **energ_ );

		/**
		Get forces apply on the atoms of the molecular system
		@param n_atoms number of atoms on which a force is applying
		@param atom_list atom id arrays on which a force is applying(the array have n_atoms integers)
		@param force_list x y z force components applying on each atom in the atom_list id array (this array must have n_atoms*3 floats)
		@return if energy is avalaible return 1 else return 0
		*/
		extern int   IIMD_get_forces        ( imd_int32 *n_atoms, imd_int32 **atom_list, float     **force_list);	

		/**
		Terminate properly a MDDriver connection
		*/
		extern void  IIMD_terminate         ( );



#ifdef __cplusplus
		}
#endif

#endif

/** @mainpage MDDriver Documentation
*@htmlinclude mddriver.html
*/



