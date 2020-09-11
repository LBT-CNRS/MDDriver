/**@file imd_interface.c
* @brief Handle IMD routines and deal with VMD protocol
* @author Marc Baaden <baaden@smplinux.de>
* @author Olivier Delalande
* @author Nicolas Ferey
* @author Gilles Grasseau
* @author This source code was adapted from the VMD/NAMD packages.
* @date   2007-8
*/
/*
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
* This software is a computer program whose purpose is to provide a generic
* interface with IMD. It was based on NAMD routines(MasterOutputIMD).
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
* $Id: imd_interface.c,v 1.8 2008-06-26 13:44:13 mdd Exp $
*
* References :
* If you use this code, could you please cite one of these references :
* O. Delalande, N. Ferey, G. Grasseau and M. Baaden : "Complex Molecular Assemblies at hand via Interactive Simulations", 2009, Journal of Computational Chemistry 2009.
* N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "A VR framework for interacting with molecular simulations", 2008, in proceedings of ACM symposium on Virtual reality software and technology (ACM - VRST'08).
* N. Ferey, O. Delalande, G. Grasseau and M. Baaden : "From Interactive to Immersive Molecular Dynamics", in Proceedings of the international Workshop on Virtual Reality and Physical Simulation (Eurographics - VRIPHYS'08).
*/

//=======================================================
//
//   Interfacing routines between Molecular Dynamics codes
//   and IMD start with "IIMD_" prefix
//
//=======================================================

#include "imd_interface.h"
#include "imd.h"
#include "vmdsock.h"
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#include <netdb.h>
#endif

//-------------------------------------------------------
//
// External data
//
//-------------------------------------------------------
imd_int32 imd_event;
imd_int64 imd_value;

//-------------------------------------------------------
//
// Internal data
//
//-------------------------------------------------------

// If (IMDserver != 0) server mode else client mode
static int IMDserver = 0;

// Flag to force (or not) waiting communication every iterations
// The function is a blocking function if IMDwait != 0
//          and is a non-blocking function if IMDwait = 0
static int IMDwait = 0;

// Flag to ignore (or not) forces
static imd_int32 IMDignore = 0;

// Connection port number
static imd_int32 IMDport = 3000 ;

// Message level
//   0 -> Only error messages
//   1 -> Force list + (0)
//   2 -> All messages
//   3 -> All messages
static imd_int32 IMDmsg = 0 ;

// Log file for MDDriver messages
static FILE     *IMDlog = NULL;

// Swap is need if IMDswap=1
static int IMDswap;

// Server socket handle
static void *sock  = 0;
static int paused  = 0;       // Receices the timeout value when IMD_PAUSE
static int timeout = 3600;

// Reserved fields for communications
static imd_int32    vmd_length = 0;
static imd_int32    vmd_Nforces = 0;
static imd_int32    vmd_Ncoords = 0;
static imd_int32   *vmd_atoms  = 0;
static float       *vmd_forces = 0;
static float       *vmd_coords = 0;
static IMDEnergies  vmd_energies;
static imd_int32    vmd_new_coords = 0;
static imd_int32    vmd_new_energies = 0;

//-------------------------------------------------------
//
//  Socket list utilities
//  pauserefix: SL_
//
//-------------------------------------------------------

static void** SL_SocketList = 0 ;
static int    SL_NSockets   = 0 ;

static void SL_AddSocket(void *s)
{
	void** new_socket_list = (void**) malloc( sizeof(void*) * (SL_NSockets + 1));
	int i;

	if (new_socket_list == 0)
		fprintf(IMDlog, "MDDriver > Socket list allocation failed\n");

	for (i = 0; i < SL_NSockets; i++)
	{
		new_socket_list[i] = SL_SocketList[i];
	}
	free(SL_SocketList);
	SL_SocketList = new_socket_list;
	SL_SocketList[SL_NSockets] = s;
	SL_NSockets += 1;
}

static void SL_DelSocket( int index )
{
	void** new_socket_list;

	if ( index < SL_NSockets )
	{
		if (SL_NSockets == 1)
		{
			vmdsock_destroy(SL_SocketList[index]);
			new_socket_list = 0;
		}
		else
		{
			int i;
			new_socket_list = (void**) malloc( sizeof(void*) * (SL_NSockets - 1));

			if ( (new_socket_list == 0) && (SL_NSockets != 1) )
				fprintf(IMDlog, "MDDriver >      Socket list allocation failed\n");

			for (i = 0; i < index; i++)
			{
				new_socket_list[i] = SL_SocketList[i];
			}
			vmdsock_destroy(SL_SocketList[index]);

			for (i = index + 1; i < SL_NSockets; i++)
			{
				new_socket_list[i - 1] = SL_SocketList[i];
			}
		}

		free(SL_SocketList);
		SL_SocketList = new_socket_list;
		SL_NSockets -= 1;
	}
}

static void SL_DelAllSocket( )
{
	int i;
	for (i = 1; i < SL_NSockets; i++)
	{
		vmdsock_destroy( SL_SocketList[i] );
	}
	if (SL_SocketList) free(SL_SocketList);
}


//-------------------------------------------------------
//
//  Internal functions/utilities
//
//-------------------------------------------------------

#ifdef __MACOSX__

//gethostbyname_r n'existe pas sous macosx
static int find_free_port(void *sock, const char* hostname, int defport)
{
	int port;
	struct hostent *hp;
	imd_int32 ip4 = INADDR_ANY;
	int error;
	if ( (hostname[0] == '\0') || (hostname == 0) )
	{
		ip4 = INADDR_ANY;
	}
	else
	{
		hp = 0;
		hp = getipnodebyname(hostname, AF_INET, AI_DEFAULT, &error);
		if (hp == 0)
			ip4 = INADDR_ANY;
		else
			memcpy((void*)&ip4, (void*) (hp->h_addr_list[0]), sizeof(imd_int32));
		free( hp);
	}

	if (vmdsock_host_bind(sock, ip4, defport) == 0)
		return defport; // success
	for (port = 1025; port < 4096; port++)
		if (vmdsock_host_bind(sock, ip4, port) == 0)
			return port;
	return -1;
}
#elif defined(_WIN32)
static int find_free_port(void *sock, const char* hostname, int defport)
{
	int port;
	struct hostent ent;
	char* buffer;
	int buflen = 1024;
	int rc, val;
	struct hostent *hp;
	imd_int32 ip4 = INADDR_ANY;
	if ((hostname[0] == '\0') || (hostname == 0))
	{
		ip4 = INADDR_ANY;
	}
	else
	{
		buffer = malloc(buflen);
		hp = 0;
		hp = gethostbyname(hostname);

		if (hp == 0)
			ip4 = INADDR_ANY;
		else
			memcpy((void*)&ip4, (void*)(ent.h_addr_list[0]), sizeof(imd_int32));

		if (hp == 0)
			ip4 = INADDR_ANY;
		else
			memcpy((void*)&ip4, (void*)(hp->h_addr_list[0]), sizeof(imd_int32));

		free(buffer);
	}

	if (vmdsock_host_bind(sock, ip4, defport) == 0) return defport; // success
	for (port = 1025; port < 4096; port++)
		if (vmdsock_host_bind(sock, ip4, port) == 0) return port;
	return -1;
}
#else
static int find_free_port(void *sock, const char* hostname, int defport)
{
	int port;
	struct hostent ent;
	char* buffer ;
	int buflen = 1024;
	int rc, val;
	struct hostent *hp;
	imd_int32 ip4 = INADDR_ANY;
	if ( (hostname[0] == '\0') || (hostname == 0) )
	{
		ip4 = INADDR_ANY;
	}
	else
	{
		buffer = malloc( buflen );
		hp = 0;
		rc = gethostbyname_r(hostname, &ent, buffer, buflen, &hp, &val);


		if (hp == 0)
			ip4 = INADDR_ANY;
		else
			memcpy((void*)&ip4, (void*) (ent.h_addr_list[0]), sizeof(imd_int32));

		if (hp == 0)
			ip4 = INADDR_ANY;
		else
			memcpy((void*)&ip4, (void*) (hp->h_addr_list[0]), sizeof(imd_int32));

		free( buffer);
	}

	if (vmdsock_host_bind(sock, ip4, defport) == 0) return defport; // success
	for (port = 1025; port < 4096; port++)
		if (vmdsock_host_bind(sock, ip4, port) == 0) return port;
	return -1;
}
#endif

static int initiate_connection(void *s)
{
	if (imd_handshake(s))
	{
		fprintf(IMDlog, "MDDriver >      handshake failed\n");
		return 0;
	}

	// Wait one second, then see if VMD has responded.
	imd_int32 length;
	if (vmdsock_selread(s, 1) != 1 || imd_recv_header(s, &length) != IMD_GO)
	{
		fprintf(IMDlog,
		        "MDDriver >      Incompatible Interactive MD, use VMD v1.4b2 or higher\n");
		return 0;
	}
	return 1;
}

void disconnect( void *sock_ )
{
	if (sock_)
	{
		imd_disconnect( sock_ );
		vmdsock_shutdown(sock_);
		vmdsock_destroy(sock_);
	}
}

void IIMD_treatprotocol()
{
	IMDType    type;
	int        n_forces = 0;      // Number of non-zero forces
	int        i_client;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);
	if ( IMDserver )
	{
		// Loop through each socket one at a time.  By doing this, rather than
		// polling all sockets at once, NAMD only has to keep up with one IMD
		// connection; if it tried to read from all of them, it could more easily
		// fall behind and never finish draining all the sockets.
		// It would be better to have a system where VMD couldn't DOS NAMD by
		// spamming it with messages, but in practice NAMD is able to keep up with
		// VMD's send rate.
		for (i_client = 0; i_client < SL_NSockets; i_client++)
		{
			void *clientsock = SL_SocketList[i_client];
			int i;
			//fprintf( IMDlog, "MDDriver > ---- client  \n");
			// Not used here - 3 lines
			// Read once the socket
			// while (vmdsock_selread(clientsock,0) > 0 || paused)
			//      {    // Drain the socket

			if (vmdsock_selread(clientsock, paused ) > 0 || paused)
			{
				type = imd_recv_header(clientsock, &vmd_length);

				if (IMDmsg >= 1)
					fprintf(IMDlog, "MDDriver >      Processing event type %d  (client = %d, size = %d) \n", type, i_client, vmd_length);
				switch (type)
				{
				case IMD_MDCOMM:
					// List of atoms and forces are expected
					//  + atom list consist of array of vmd_length integer-32
					//  + force list consist of array of 3*vmd_length float

					if (IMDmsg >= 1)
						fprintf(IMDlog, "MDDriver >      Received %d forces \n", vmd_length);

					if ( vmd_length != 0 )
					{

						if ( vmd_forces != 0 )
						{
							free( vmd_atoms );
							free( vmd_forces );
							vmd_atoms = 0; vmd_forces = 0;
						}

						vmd_atoms  = (imd_int32 *) malloc( sizeof(imd_int32) * vmd_length     );
						vmd_forces = (float *)     malloc( sizeof(float)     * vmd_length * 3 );

						if (imd_recv_mdcomm(clientsock, vmd_length, vmd_atoms, vmd_forces))
						{
							fprintf(IMDlog, "MDDriver >      Error reading IMD forces, killing connection\n");
							SL_DelSocket(i_client);
						}

						for (i = 0; i < vmd_length; i++)
						{
							n_forces = n_forces + (  (vmd_forces[i * 3]   != 0)
							                         || (vmd_forces[i * 3 + 1] != 0)
							                         || (vmd_forces[i * 3 + 2] != 0)
							                      );
						}
					}

					if ( (vmd_length == 0) || ( n_forces == 0) )
					{
						fprintf(IMDlog, "MDDriver >      Warning no force \n");
						free( vmd_atoms );
						free( vmd_forces );
						vmd_atoms = 0;  vmd_forces = 0;
						vmd_length = 0; n_forces = 0;
					}

					vmd_Nforces =  n_forces;

					if (IMDignore)
					{
						fprintf(IMDlog, "MDDriver >      Ignoring IMD forces due to IMDignore\n");
					}

					if (IMDmsg > 0)
					{
						fprintf(IMDlog, "MDDriver >      \n");
						fprintf(IMDlog, "MDDriver >        IMD Forces received from client \n");
						fprintf(IMDlog, "MDDriver >        ------------------------------- \n");
						fprintf(IMDlog, "MDDriver >      \n");

						fprintf(IMDlog, "MDDriver >      Number of forces to apply %d\n",
						        vmd_Nforces);
						for (i = 0; i < vmd_Nforces; i++)
						{
							fprintf(IMDlog, "MDDriver >      %d  --> Atom %d : %f %f %f\n", i, vmd_atoms[i],  vmd_forces[i * 3], vmd_forces[i * 3 + 1], vmd_forces[i * 3 + 2]);
						}
						fprintf(IMDlog, "MDDriver >      \n");

					}
					break;

				case IMD_TRATE:
					imd_event = IMD_TRATE;
					imd_value = vmd_length;
					fprintf(IMDlog, "MDDriver >      IMDfreq = %d\n", vmd_length );
					break;

				case IMD_PAUSE:
					fprintf(IMDlog, "MDDriver >      Pausing IMD\n");;

					fprintf(IMDlog, "MDDriver >      \n");
					imd_event = IMD_PAUSE;
					imd_value = 0;
					if ( paused == 0 )
					{
						fprintf(IMDlog, "MDDriver >      Pausing IMD\n");;
						paused = timeout;
					}
					else
					{
						fprintf(IMDlog, "MDDriver >      Resuming IMD\n");
						paused = 0;
					}
					fprintf(IMDlog, "MDDriver >      \n");

					break;

				case IMD_IOERROR:
					fprintf(IMDlog, "MDDriver >      connection lost\n");
					break;

				case IMD_DISCONNECT:
					fprintf(IMDlog, "MDDriver >      \n");
					fprintf(IMDlog, "MDDriver >      Connection detached\n");
					SL_DelSocket(i_client);
					break;

				case IMD_KILL:
					if (IMDignore)
					{
						fprintf(IMDlog, "MDDriver >      Ignoring IMD kill due to IMDignore\n");
					}
					else
					{
						fprintf(IMDlog, "MDDriver >      Received IMD kill from client\n");
						imd_event = IMD_KILL;
						imd_value = 0;
						// Avoid waiting for another connection
						IMDwait = 0;
					}
					break;

				case IMD_ENERGIES:
				{
					IMDEnergies dummy;
					imd_recv_energies(clientsock, &dummy);

					break;
				}

				case IMD_FCOORDS:
					//
					vmd_coords = (float* )malloc( sizeof(float) * 3 * vmd_length );
					imd_recv_fcoords(clientsock, vmd_length, vmd_coords);
					free(vmd_coords);
					break;
				default: ;
				}
			}
		}
	}

	// Client mode
	// ===========
	else
	{
		while ( sock != 0 && vmdsock_selread(sock, 0))
		{
			type = imd_recv_header(sock, &vmd_length);
			switch (type)
			{
			case IMD_FCOORDS:
				if (vmd_Ncoords < vmd_length)
				{
					if (vmd_Ncoords != 0)
						free( vmd_coords );
					vmd_coords = (float*) malloc( 3 * vmd_length * sizeof(float));
				}
				vmd_Ncoords = vmd_length;
				if (imd_recv_fcoords(sock, vmd_Ncoords, vmd_coords))
				{
					fprintf(IMDlog, "MDDriver >      \n");
					fprintf(IMDlog, "MDDriver >      Cannot read coordinates - disconnecting \n");
					disconnect( sock );
				}
				else
				{
					if (IMDmsg >= 1 )
					{
						fprintf(IMDlog, "MDDriver >      Received %d positions\n", vmd_Ncoords );
					}
					if ( IMDswap ) imd_swap4( (char *) vmd_coords, 3 * vmd_Ncoords);
					vmd_new_coords = 1;
				};
				break;
			case IMD_ENERGIES:
				if (imd_recv_energies(sock, &vmd_energies))
				{
					fprintf(IMDlog, "MDDriver >      \n");
					fprintf(IMDlog, "MDDriver >      Cannot read energies - disconnecting \n");
					disconnect( sock );
				}
				else
				{
					if ( IMDswap ) imd_swap4((char *) &vmd_energies, sizeof(vmd_energies) / 4);
					vmd_new_energies = 1;
				}
				break;
			case IMD_DISCONNECT:
				fprintf(IMDlog, "MDDriver >      \n");
				fprintf(IMDlog, "MDDriver >      Connection detached\n");
				vmdsock_shutdown( sock );
				vmdsock_destroy( sock );
				sock = 0;
				break;
			case IMD_IOERROR:
				fprintf(IMDlog, "MDDriver >      \n");
				fprintf(IMDlog, "MDDriver >      IMD_IOERROR\n");
				disconnect( sock );
				sock = 0;
				break;

			default:
				break;
			}
		}
	}
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}



//-------------------------------------------------------
//
//   Interface routines between Molecular Dynamics codes
//   and IMD.
//   Start with "IIMD_" prefix
//
//-------------------------------------------------------

// if *hostname = 0
FILE *IIMD_init( const char  *hostname,     imd_int32   *mode,  imd_int32   *IMDwait_,  imd_int32   *IMDport_,  imd_int32   *IMDmsg_,   const char  *IMDlogname_    )
{
	size_t blen = 1024;
	char hname[1024];
	const char *str;

	if ( IMDlogname_ == 0 )
	{
		IMDlog = stderr;
	}
	else
	{
		IMDlog = fopen ( IMDlogname_, "w");

		if ( IMDlog == 0 )
		{
			fprintf(stderr,  "MDDriver >      Bad filename, using stderr\n");
			IMDlog = stderr;
		}
	}

	IMDmsg = *IMDmsg_;

	// IMDmsg < 0 force to display to stderr (IMDlog=stderr)
	if ( IMDmsg < 0)
	{
		IMDmsg = -IMDmsg;
		IMDlog = stderr;
		fprintf(IMDlog, "MDDriver >      Negative IMDmsg - Setting IMDlog=stderr \n");
	}

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	IMDwait   = *IMDwait_;  //  IMDwait = 1 -> blocking
	IMDignore = 0;

	if ( vmdsock_init() )
	{
		fprintf(IMDlog, "MDDriver >      Unable to initialize socket interface for IMD.\n");
		IMDignore =  1;
	}
	sock = vmdsock_create();
	IMDport = *IMDport_;
	// Server Mode
	if ( *mode )
	{
		IMDserver = 1;
		if ( IMDport  <= 1024 )
		{
			fprintf(IMDlog, "MDDriver >      Bad port number (using %d) \n", IMDport);
			IMDport = 3000;
		}

		int newport = find_free_port(sock, hostname, IMDport);
		if (IMDport != newport)
		{
			fprintf(IMDlog, "MDDriver >      Interactive MD failed to bind to port %d \n", IMDport);
			IMDport = newport;
		}
		if (newport < 0)
		{
			vmdsock_destroy(sock);
			fprintf(IMDlog, "MDDriver >      Interactive MD failed to find free port \n");
		}
		else
		{
			IMDport = newport;
			str = hostname;
			if ( (hostname == 0) || (hostname[0] == '\0') )
			{
				gethostname( hname, blen );
				str = hname;
			}
			fprintf(IMDlog, "MDDriver >      Interactive MD bind to /%s/%d \n", str, IMDport );
		}

		fflush( IMDlog );

		// Listening incomming requests on sock
		vmdsock_listen(sock);
	}
	// Client mode
	else
	{
		if ( vmdsock_connect(sock, hostname, IMDport) < 0 )
		{
			fprintf(IMDlog, "MDDriver >      Unable to connect to server %s:%d   %d\n", hostname, IMDport);
			vmdsock_destroy(sock);
			sock = 0;
		}
		else
		{
			IMDswap = imd_recv_handshake(sock);
			if ( IMDswap == 0)
			{
				if (IMDmsg >= 1)
					fprintf(IMDlog, "MDDriver >      Same endian machines\n");
			}
			else if ( IMDswap == 1)
			{
				if (IMDmsg >= 1)
					fprintf(IMDlog, "MDDriver >      Different endian machines\n");
			}
			else
			{
				if (IMDmsg >= 1)
					fprintf(IMDlog, "MDDriver >      Unknown endian machine - disconnecting\n");
				if (sock)
				{
					imd_disconnect(sock);
					vmdsock_shutdown(sock);
					vmdsock_destroy(sock);
					sock = 0;
				}
			}
		}
	}

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);

	imd_event = -1;
	return ( IMDlog );
}

int IIMD_probeconnection()
{
	int code = 0;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);
	if ( IMDserver )
	{
		// Check for incoming connection
		do
		{
			int rc;

			// Test
			if ( IMDwait && (SL_NSockets == 0) )
			{
				// No established connection
				fprintf(IMDlog, "MDDriver >      Awaiting connection\n");
				do
				{
					rc = vmdsock_selread(sock, 3600);
				} while (rc <= 0);
			}
			else
			{
				rc = vmdsock_selread(sock, 0);
			}
			if (rc > 0)
			{
				void *clientsock = vmdsock_accept(sock);
				if (!clientsock)
				{
					fprintf(IMDlog, "MDDriver >      socket accept failed\n");
					code = 1;
				}
				else
				{
					if ( ! initiate_connection(clientsock) )
					{
						fprintf(IMDlog, "MDDriver >      connection failed\n");
						vmdsock_destroy(clientsock);
					}
					else
					{
						fprintf(IMDlog, "MDDriver >      connection opened\n");
						SL_AddSocket( clientsock );
					}
				}
			}
		}
		while (IMDwait && (SL_NSockets == 0));
	}
	// Client mode
	else
	{
		if (sock != 0) code = 1;
	}
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);

	return code;
}

void IIMD_treatevent( )
{
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	IIMD_probeconnection();

	// Check/get new events from VMD
	IIMD_treatprotocol();

	// Right now I don't check to see if any new forces were obtained.
	// An optimization would be cache the results message.  However, there
	// would still be copying since it looks like the messages get deleted
	// by the receiver.

	/* set our arrays to be big enough to hold all of the forces */

	// Update the forces if necessary

	// Return
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_energies(const IMDEnergies *energies)
{
	int i;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	if (IMDmsg >= 2)
	{
		fprintf(IMDlog, "MDDriver > \n");
		fprintf(IMDlog, "MDDriver >      Sent energies (step %d) \n", vmd_energies.tstep);
		fprintf(IMDlog, "MDDriver >      ------------------------------------------ \n");
		fprintf(IMDlog, "MDDriver >      [Cal] for energies      \n" );
		fprintf(IMDlog, "MDDriver >      [K] for the temperature \n" );
		fprintf(IMDlog, "MDDriver >      [Bar] for pressure      \n" );
		fprintf(IMDlog, "MDDriver >      Time step         [ ]   %12d\n",   vmd_energies.tstep);
		fprintf(IMDlog, "MDDriver >      Temperature       [K]   %12.5e\n", vmd_energies.T);
		fprintf(IMDlog, "MDDriver >      Total E.          [Cal] %12.5e\n", energies->Etot);
		fprintf(IMDlog, "MDDriver >      Bond E.           [Cal] %12.5e\n", vmd_energies.Ebond);
		fprintf(IMDlog, "MDDriver >      Angle E.          [Cal] %12.5e\n", energies->Eangle);
		fprintf(IMDlog, "MDDriver >      Potential E.      [Cal] %12.5e\n", energies->Epot);
		fprintf(IMDlog, "MDDriver >      Dihedrale E.      [Cal] %12.5e\n", energies->Edihe);
		fprintf(IMDlog, "MDDriver >      Improp. Dihed. E. [Cal] %12.5e\n", energies->Eimpr);
		fprintf(IMDlog, "MDDriver >      Van der Waals E.  [Cal] %12.5e\n", energies->Evdw);
		fprintf(IMDlog, "MDDriver >      Electrostatic. E. [Cal] %12.5e\n", energies->Eelec);
		fprintf(IMDlog, "MDDriver >      \n");
	}

	for (i = 0; i < SL_NSockets; i++)
	{
		void *clientsock = SL_SocketList[i];
		if (!clientsock || !vmdsock_selwrite(clientsock, 0)) continue;
		imd_send_energies(clientsock, energies);
	}

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_energies_array(const imd_int32 *tstep_, const float *energies_)
{
	int i;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	IMDEnergies energies;
	energies.tstep  = *tstep_;
	energies.T      = energies_[0];
	energies.Etot   = energies_[1];
	energies.Epot   = energies_[2];
	energies.Evdw   = energies_[3];
	energies.Eelec  = energies_[4];
	energies.Ebond  = energies_[5];
	energies.Eangle = energies_[6];
	energies.Edihe  = energies_[7];
	energies.Eimpr  = energies_[8];

	for (i = 0; i < SL_NSockets; i++)
	{
		void *clientsock = SL_SocketList[i];
		if (!clientsock || !vmdsock_selwrite(clientsock, 0))
			continue;
		imd_send_energies( clientsock, &energies );
	}

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_coords(const int *N_p, const float *coords)
{
	int N = *N_p;
	int i;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	for (i = 0; i < SL_NSockets; i++)
	{
		void *clientsock =  SL_SocketList[i];
		if (!clientsock || !vmdsock_selwrite(clientsock, 0))
			continue;
		imd_send_fcoords(clientsock, N, (float *)coords);
	}

	if (IMDmsg >= 2)
	{
		fprintf(IMDlog, "MDDriver >      \n");
		fprintf(IMDlog, "MDDriver >      Sent positions (step %d) \n", vmd_energies.tstep);
		fprintf(IMDlog, "MDDriver >      ------------------------------------------ \n");
		fprintf(IMDlog, "MDDriver >      Number of sent positions  %d\n", N);
		for (i = 0; i < N; i++)
		{
			fprintf(IMDlog, "MDDriver >      Atom position %d : %f %f %f\n", i,  coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2]);
		}
	}

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_forces(const int *N_p, int* AtomIndex, float *forces)
{
	int N = *N_p;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	if (IMDmsg >= 2)
	{
		fprintf(IMDlog, "MDDriver >      \n");
		fprintf(IMDlog, "MDDriver >      Sent forces \n");
		fprintf(IMDlog, "MDDriver >      ------------------------------- \n");
		fprintf(IMDlog, "MDDriver >      \n");
		fprintf(IMDlog, "MDDriver >	 Number of sent forces S %d \n" , N);
		unsigned fx = 0;
		unsigned fy = 1;
		unsigned fz = 2;
		unsigned i;

		for (i = 0; i < N; i++)
		{
			fx = 3 * i;
			fy = 3 * i + 1;
			fz = 3 * i + 2;
			fprintf( IMDlog, "MDDriver >      Atom force %d : %f %f %f\n", AtomIndex[i], forces[fx], forces[fy], forces[fz]);
		}
	}
	if (sock) //&& vmdsock_selwrite(sock, 0))//XM: not sure why there is this
	{
		if ( IMDswap )
		{
			imd_swap4( AtomIndex, N);
			imd_swap4( forces, 3 * N);
		}
		if (imd_send_mdcomm( sock, N, AtomIndex, forces) && IMDmsg >= 1) {
			fprintf( IMDlog, "MDDriver > ---- Failed to send forces in %s\n", __FUNCTION__);
			IIMD_send_disconnect();
		}
	}

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_disconnect()
{
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	imd_disconnect( sock );

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_pause()
{
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);
	imd_pause( sock );
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_kill()
{
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	imd_kill( sock );

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}

void IIMD_send_trate(imd_int32 *trate)
{
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	imd_trate( sock , *trate );

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}


// "coords" must be allocated with 3 * Atom number
void IIMD_recv_coords(const int *N_p, float *coords)
{
	int N = *N_p;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	if ( imd_recv_fcoords(sock, N, coords ) != 0)
	{
		fprintf(stderr,  "MDDriver >      Bad number of coordinates -  disconnecting\n");
		imd_disconnect(sock);
		vmdsock_shutdown(sock);
		vmdsock_destroy(sock);
		sock = 0;
	}
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
}


// ?? A revoir a cause du fortran
int IIMD_get_forces( imd_int32 *n_forces_, imd_int32 **atom_list_, float **force_list_)
{
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	*n_forces_  = vmd_Nforces;
	*atom_list_ = vmd_atoms;
	*force_list_ = vmd_forces;

	if (IMDmsg >= 2)
	{
		fprintf(IMDlog, "MDDriver >      \n");
		fprintf(IMDlog, "MDDriver >      Get forces (step %d) \n", vmd_energies.tstep);
		fprintf(IMDlog, "MDDriver >      ------------------------------------------ \n");
		fprintf(IMDlog, "MDDriver >      \n");

		fprintf( IMDlog, "MDDriver >      Number of get forces %d \n" , *n_forces_);
		unsigned f = 0;
		unsigned i;

		for (i = 0; i < vmd_Nforces; i++)
		{
// MB: the line below creates a warning, because the order of the 3 increases of f++ is undefined. I rewrote it to avoid the warning
//     and obtain a well defined situation -- eg lower risks for -O6 etc.
//          fprintf( IMDlog, "MDDriver >      Atom force %d : %f %f %f\n",vmd_atoms[i],vmd_forces[f++],vmd_forces[f++],vmd_forces[f++]);
			fprintf( IMDlog, "MDDriver >      Atom force %d : %f ", vmd_atoms[i], vmd_forces[f++]);
			fprintf( IMDlog, "%f ", vmd_forces[f++]);
			fprintf( IMDlog, "%f\n", vmd_forces[f++]);
		}
	}

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
	return (  vmd_Nforces );
}

int IIMD_get_coords( imd_int32 *n_atoms, float **coords)
{
	int rc = 0;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);
	if ( vmd_new_coords )
	{
		*n_atoms = vmd_Ncoords;
		*coords  = vmd_coords;
		rc = vmd_Ncoords;

		if (IMDmsg >= 2)
		{
			fprintf(IMDlog, "MDDriver >      \n");
			fprintf(IMDlog, "MDDriver >      Get positions (step %d) \n", vmd_energies.tstep);
			fprintf(IMDlog, "MDDriver >      ------------------------------------------ \n");
			unsigned i = 0;
			fprintf(IMDlog, "MDDriver >      Number of get positions  %d\n", vmd_Ncoords);
			for (i = 0; i < vmd_Ncoords; i++)
			{
				fprintf(IMDlog, "MDDriver >      Atom position %d : %f %f %f\n", i,  vmd_coords[i * 3], vmd_coords[i * 3 + 1], vmd_coords[i * 3 + 2]);
			}
		}
	}

	vmd_new_coords = 0;
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
	return rc;
}

int IIMD_get_energies( IMDEnergies **energ_ )
{
	int rc = 0;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	//Update the energ pointer even if no new energies
	*energ_ = &vmd_energies;

	if ( vmd_new_energies )
	{
		rc = 1 ;

		if (IMDmsg >= 2)
		{
			fprintf(IMDlog, "MDDriver >      \n");
			fprintf(IMDlog, "MDDriver >      Get energies (step %d) \n", vmd_energies.tstep);
			fprintf(IMDlog, "MDDriver >      ------------------------------------------ \n");
			fprintf(IMDlog, "MDDriver >      [Cal] for energies      \n" );
			fprintf(IMDlog, "MDDriver >      [K] for the temperature \n" );
			fprintf(IMDlog, "MDDriver >      [Bar] for pressure      \n" );
			fprintf(IMDlog, "MDDriver >      Time step         [ ]   %12d\n",   vmd_energies.tstep);
			fprintf(IMDlog, "MDDriver >      Temperature       [K]   %12.5e\n", vmd_energies.T);
			fprintf(IMDlog, "MDDriver >      Total E.          [Cal] %12.5e\n", vmd_energies.Etot);
			fprintf(IMDlog, "MDDriver >      Bond E.           [Cal] %12.5e\n", vmd_energies.Ebond);
			fprintf(IMDlog, "MDDriver >      Angle E.          [Cal] %12.5e\n", vmd_energies.Eangle);
			fprintf(IMDlog, "MDDriver >      Potential E.      [Cal] %12.5e\n", vmd_energies.Epot);
			fprintf(IMDlog, "MDDriver >      Dihedrale E.      [Cal] %12.5e\n", vmd_energies.Edihe);
			fprintf(IMDlog, "MDDriver >      Improp. Dihed. E. [Cal] %12.5e\n", vmd_energies.Eimpr);
			fprintf(IMDlog, "MDDriver >      Van der Waals E.  [Cal] %12.5e\n", vmd_energies.Evdw);
			fprintf(IMDlog, "MDDriver >      Electrostatic. E. [Cal] %12.5e\n", vmd_energies.Eelec);
			fprintf(IMDlog, "MDDriver >      \n");
		}
	}


	vmd_new_energies = 0;
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);
	return rc;
}


void IIMD_terminate()
{
	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Entering in %s\n", __FUNCTION__);

	// if (vmd_coords){
	// free( vmd_coords );
	// }
	vmd_Ncoords = 0;
	vmd_length = 0;
	vmd_Nforces = 0;
	vmd_atoms  = 0;
	vmd_forces = 0;
	vmd_coords = 0;
	vmd_new_coords = 0;
	vmd_new_energies = 0;
	if (sock) {
		vmdsock_shutdown(sock);
		vmdsock_destroy(sock);
	}
	SL_DelAllSocket();
	sock = 0;

	if (IMDmsg >= 1)
		fprintf( IMDlog, "MDDriver > ---- Leaving     %s\n", __FUNCTION__);

	if ( IMDlog != stderr )
		fclose(IMDlog);
}
