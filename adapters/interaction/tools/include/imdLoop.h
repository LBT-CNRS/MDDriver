#ifndef _IMD_LOOP_H_
#define _IMD_LOOP_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "imd_interface.h"

#define HOSTNAMESTRINGSIZE 256 

class imdLoop
	{
	public : 
		static const double CaloryToJoule  ; 
		static const double CaloryAToJouleNM ;
		static const double JouleToCalory   ;
		static const double NmToAngstrom    ;
		static const unsigned dim;
		
		imdLoop(const char * hostname,int port);
		virtual ~imdLoop();

		void start();
		int setForces(int nbforce,int * atomslist, float * forceslist);
		int getCoords(float * coordinates);
		int getCoords(float * coordinates,int atomsnumber);

		virtual void updateForces() ;
		
		int nb_coords ;
		float * coords;
		int  nb_forces ;                 
		int   * forces_atoms_list;
		float  * forces_list;

		IMDEnergies *p_energies;            // Energies


	private : 
		static int iterate( imdLoop * imdl);
		static void init( imdLoop * imdl);
		static void end( imdLoop * imdl);

		static void  * run(void * userdata);
		void setTimeSleep(int usleep);

		char hostname[HOSTNAMESTRINGSIZE];
		int port  ;
		
		int wait   ;                     // Connection configuration
		int IMDmsg ;
		int mode ;
		bool pause  ;
	
		int cont   ;                     // Main loop control
		int ffreq  ;
	

		FILE *MYIMDlog;
	
		unsigned timestep;
		unsigned timesleep;
		pthread_t imdthreads;
		pthread_mutex_t coordmutex;
		pthread_mutex_t forcemutex;

	
	};

#endif
