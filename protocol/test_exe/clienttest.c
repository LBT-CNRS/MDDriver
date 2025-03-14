/*! \file main.c
 * \brief Template for using MDDriver with a simple "visualization" code or client code.
 * \author Marc Baaden <baaden@smplinux.de>
 * \author Olivier Delalande
 * \author Nicolas Ferey
 * \author Gilles Grasseau
 * \date   2007-8
 *
 * Copyright Centre National de la Recherche Scientifique (CNRS)
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
 * References :
 * If you use this code, could you please cite one of these references :
 * O. Delalande, N. Ferey, G. Grasseau and M. Baaden : "Complex Molecular Assemblies at hand via Interactive Simulations", 2009, Journal of Computational Chemistry 2009.
 * N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "A VR framework for interacting with molecular simulations", 2008, in proceedings of ACM symposium on Virtual reality software and technology (ACM - VRST'08).
 * N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "From Interactive to Immersive Molecular Dynamics", in Proceedings of the international Workshop on Virtual Reality and Physical Simulation (Eurographics - VRIPHYS'08).
 *
 * $Id: clienttest.c,v 1.1 2008-06-25 12:16:49 mdd Exp $
 *
 * A simple template that shows how to implement calls to MDDriver from
 * an example program (clien part)



 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include "imd_interface.h"

#define N 104
#define NDIM 3

#define N_FORCE 5

// Converting Calories x Angstrom-1
// to Joules x nm-1
const double CaloryToJoule    = 4.184;
const double CaloryAToJouleNM = 4.184 * 10.0;
const double JouleToCalory    = 1. / 4.184;
const double NmToAngstrom     = 10.0;

typedef float t_coord[3];

FILE *IMDlog;
char IMDhostname[] = "localhost";
int IMDport   = 8888;
int IMDmsg = 2;
int IMDwait   = 0;                     // Connection configuration
int IMDmode = 0; // 0 : client , 1 : server

// Return a non zero value if a keyboard event occurs
#if !defined(_WIN32)
int keyboard_event()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET( STDIN_FILENO, &fds);
}
#endif

void connection()
{
    while (!IIMD_probeconnection())
    {
        IMDlog =  IIMD_init( IMDhostname, &IMDmode, &IMDwait, &IMDport, &IMDmsg , 0 );
        IIMD_treatevent();
#if defined(_WIN32)
        Sleep(500);
#else
        usleep(500000);
#endif
    }
}

// -----------------------------------------------------------
// BELOW COMES THE MAIN PROGRAM LOOP WITH CALLS TO MDDRIVER
// -----------------------------------------------------------
int main()
{
    //  char hostname[] = "lin1.idris.fr";
    int pause  = 0;
    int log = 1;

    int N_atoms;                        // Atom positions
    t_coord *coords;

    int N_forces = N_FORCE;
    int   atoms_list[N_FORCE];//Force
    float forces_list[N_FORCE*3];

    int nb = 5;
    int intdata[5] = {6, 7, 8, 9, 10};
    float floatdata[5] = {6.0, 7.0, 8.0, 9.0, 10.0};

    // cintCS : custom int Client Server
    IMDCustomData float_send = {.dataname="cfloCS", .nb=nb, .data=&floatdata};
    IMDCustomData int_send = {.dataname="cintCS", .nb=nb, .data=&intdata};

    IMDCustomData float_get, int_get;
    float_get.dataname = (char *) malloc(sizeof(char)*8);
	int_get.dataname = (char *) malloc(sizeof(char)*8);
    
    int cont   = 1;                     // Main loop control
    int ffreq  = 10;
    int i;

    IMDEnergies *p_energies;            // Energies
    IMDGrid *p_grid; //grid
    int j;
    char key;

    atoms_list[0] = 20;
    atoms_list[1] = 0;
    atoms_list[2] = 8;
    atoms_list[3] = 4;
    atoms_list[4] = 2;

    forces_list[0*3+0] = 10.0;
    forces_list[0*3+1] = -1.0;
    forces_list[0*3+2] = 1.0;

    forces_list[1*3+0] = 2.0;
    forces_list[1*3+1] = -1.0;
    forces_list[1*3+2] = 1.0;

    forces_list[2*3+0] = 4.0;
    forces_list[2*3+1] = -1.0;
    forces_list[2*3+2] = 1.0;

    forces_list[3*3+0] = 8.0;
    forces_list[3*3+1] = -1.0;
    forces_list[3*3+2] = 1.0;

    forces_list[4*3+0] = 10.0;
    forces_list[4*3+1] = 6.0;
    forces_list[4*3+2] = 1.0;

    connection();

    i = 0;
    while ( cont )
    {
        IIMD_treatprotocol();
        if ( !pause )
        {
            // New atom positions
            if ( IIMD_get_coords( &N_atoms, (float **) &coords ) )
            {
                if (log)
                {   //coords = *p_coords;
                    fprintf(IMDlog, "MYMDD > \n");
                    fprintf(IMDlog, "MYMDD > Get %d atoms (Time step=%d)\n", N_atoms, i);
                    fprintf(IMDlog, "MYMDD > ================================\n");
                    fprintf(IMDlog, "MYMDD > \n");
                    fprintf(IMDlog, "MYMDD >  Force list (10 first atoms and the last one)\n");
                    fprintf(IMDlog, "MYMDD > AtomID      x        y        z      \n");
                    fprintf(IMDlog, "MYMDD > -------------------------------------\n");
                    for (j = 0; j < 10; j++)
                    {
                        fprintf(IMDlog, "MYMDD > %7d %8.2e %8.2e %8.2e\n", j, coords [j][0], coords[j][1], coords[j][2] );
                    }
                    j = N_atoms - 1;
                    fprintf(IMDlog, "MYMDD > %7d %8.2e %8.2e %8.2e\n", j, coords[j][0], coords[j][1], coords [j][2] );
                }
            }

            // New energies
            if ( IIMD_get_energies( &p_energies ) )
            {
                if (log)
                {
                    fprintf(IMDlog, "MYMDD > \n");
                    fprintf(IMDlog, "MYMDD > Receive energies (Time step=%d)\n" , i);
                    fprintf(IMDlog, "MYMDD > ================================\n");
                    fprintf(IMDlog, "MYMDD >   \n");
                    fprintf(IMDlog, "MYMDD >   MYPROGRAM Energy List (%d) \n", 99 );
                    fprintf(IMDlog, "MYMDD >   [Cal] for energies, [K] for the temperature \n" );
                    fprintf(IMDlog, "MYMDD >   [Bar] for pressure\n" );
                    fprintf(IMDlog, "MYMDD >   ------------------------------------------ \n");
                    fprintf(IMDlog, "MYMDD >  \n");
                    fprintf(IMDlog, "MYMDD >   Simulation energy List \n" );
                    fprintf(IMDlog, "MYMDD >   --------------------\n");
                    fprintf(IMDlog, "MYMDD >   Time step         [ ]   %12d\n", p_energies->tstep);
                    fprintf(IMDlog, "MYMDD >   Temperature       [K]   %12.5e\n", p_energies->T);
                    fprintf(IMDlog, "MYMDD >   Total E.          [Cal] %12.5e\n", p_energies->Etot);
                    fprintf(IMDlog, "MYMDD >   Bond E.           [Cal] %12.5e\n", p_energies->Ebond);
                    fprintf(IMDlog, "MYMDD >   Angle E.          [Cal] %12.5e\n", p_energies->Eangle);
                    fprintf(IMDlog, "MYMDD >   Potential E.      [Cal] %12.5e\n", p_energies->Epot);
                    fprintf(IMDlog, "MYMDD >   Dihedrale E.      [Cal] %12.5e\n", p_energies->Edihe);
                    fprintf(IMDlog, "MYMDD >   Improp. Dihed. E. [Cal] %12.5e\n", p_energies->Eimpr);
                    fprintf(IMDlog, "MYMDD >   Van der Waals E.  [Cal] %12.5e\n", p_energies->Evdw);
                    fprintf(IMDlog, "MYMDD >   Electrostatic. E. [Cal] %12.5e\n", p_energies->Eelec);
                    fprintf(IMDlog, "MYMDD > \n");
                }
            }
            if ( IIMD_get_grid( &p_grid ) )
            {
                if (log)
                {
                    fprintf(IMDlog, "MYMDD > \n");
                    fprintf(IMDlog, "MYMDD > Receive grid (Time step=%d)\n" , i);
                    fprintf(IMDlog, "MYMDD > ================================\n");
                    fprintf(IMDlog, "MYMDD >   \n");
                    fprintf(IMDlog, "MYMDD >   MYPROGRAM Grid  \n");
                    fprintf(IMDlog, "MYMDD >   ------------------------------------------ \n");
                    fprintf(IMDlog, "MYMDD >  \n");
                    fprintf(IMDlog, "MYMDD >   Simulation grid info \n" );
                    fprintf(IMDlog, "MYMDD >   --------------------\n");
                    fprintf(IMDlog, "MYMDD >   Origin %f, %f, %f\n", p_grid->Xorigin, p_grid->Yorigin, p_grid->Zorigin);
                    fprintf(IMDlog, "MYMDD >   End %f, %f, %f\n", p_grid->Xend, p_grid->Yend, p_grid->Zend);
                    fprintf(IMDlog, "MYMDD >   Direction X %f, %f, %f\n", p_grid->XdirectionX, p_grid->XdirectionX, p_grid->XdirectionX);
                    fprintf(IMDlog, "MYMDD >   Direction Y %f, %f, %f\n", p_grid->XdirectionY, p_grid->XdirectionY, p_grid->XdirectionY);
                    fprintf(IMDlog, "MYMDD >   Direction Z %f, %f, %f\n", p_grid->XdirectionZ, p_grid->YdirectionZ, p_grid->ZdirectionZ);
                    fprintf(IMDlog, "MYMDD >   Number of cells  %d, %d, %d\n", p_grid->nbcellx, p_grid->nbcelly, p_grid->nbcellz);
                    fprintf(IMDlog, "MYMDD >   Number of cells  %f, %f, %f\n", p_grid->sizecellx, p_grid->sizecelly, p_grid->sizecellz);
                    fprintf(IMDlog, "MYMDD > \n");
                }
            }

            if ( IIMD_get_custom_float(&float_get.dataname,  &float_get.nb, (float**)&float_get.data) )
            {
                
                {
                  fprintf(IMDlog, "MYMDD > \n");
                  fprintf(IMDlog, "MYMDD > Receive float array (Time step=%d)\n" , i);
                  fprintf(IMDlog, "MYMDD > ================================\n");
                  fprintf(IMDlog, "MYMDD >   \n");
                  fprintf(IMDlog, "MYMDD >   MYPROGRAM float array %s \n", float_get.dataname);
                  fprintf(IMDlog, "MYMDD >   ------------------------------------------ \n");
                  for (unsigned j = 0; j < float_get.nb; j++)
                    {
                      fprintf(IMDlog, "MYMDD > float array[%d] = %f\n", j, ((float*)(float_get.data))[j]);
                    }
                  fprintf(IMDlog, "MYMDD > \n");
                }
            }
            if ( IIMD_get_custom_int(&int_get.dataname,  &int_get.nb, (int**)&int_get.data)  )
            {
                
                {
                  fprintf(IMDlog, "MYMDD > \n");
                  fprintf(IMDlog, "MYMDD > Receive int array (Time step=%d)\n" , i);
                  fprintf(IMDlog, "MYMDD > ================================\n");
                  fprintf(IMDlog, "MYMDD >   \n");
                  fprintf(IMDlog, "MYMDD >   MYPROGRAM int array %s \n", int_get.dataname);
                  fprintf(IMDlog, "MYMDD >   ------------------------------------------ \n");
                  for (unsigned j = 0; j < int_get.nb; j++)
                    {
                      fprintf(IMDlog, "MYMDD > int array[%d] = %d\n", j, ((int*)(int_get.data))[j]);
                    }
                  fprintf(IMDlog, "MYMDD > \n");
                }
            }


            // Send forces every "ffreq"
            if ( (i % ffreq) == 0 )
            {
                IIMD_send_forces ( &N_forces, atoms_list, forces_list );
            }

            IIMD_send_custom_float(float_send.dataname, &float_send.nb, float_send.data);
            IIMD_send_custom_int(int_send.dataname, &int_send.nb, int_send.data);
        }
#if !defined(_WIN32)
        // Deals with keyboard events
        if ( keyboard_event() )
        {
            key = (char) getchar();
            switch ( key )
            {
            case 'q':
                fprintf(IMDlog, "MYMDD > Stop simulation\n");
                IIMD_send_kill();
                cont = 0;
                break;
            case 'f':
                fprintf(IMDlog, "MYMDD > Increment coords frequency \n");
                ffreq++;
                IIMD_send_trate( &ffreq );
                break;
            case 'p':
                if (!pause)
                {
                    fprintf(IMDlog, "MYMDD > Starting pause \n");
                }
                else
                {
                    fprintf(IMDlog, "MYMDD > Ending pause \n");
                }
                IIMD_send_pause();
                pause = !(pause);
                break;
            case 'd':
                fprintf(IMDlog, "MYMDD > Disconnect \n");
                IIMD_send_disconnect();
                IIMD_terminate();
                cont = 0;
                break;
            }
        }
#endif
#if defined(_WIN32)
        Sleep(500);
#else
        usleep(500000);
#endif
        i++;
    }

    IIMD_terminate ();
    exit(0);
}
