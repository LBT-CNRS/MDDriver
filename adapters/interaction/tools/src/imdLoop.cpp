#include "imdLoop.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;

const double imdLoop::CaloryToJoule    = 4.184;
const double imdLoop::CaloryAToJouleNM = 4.184*10.0;
const double imdLoop::JouleToCalory    = 1.0 / 4.184;
const double imdLoop::NmToAngstrom     = 10.0;

const unsigned imdLoop::dim     = 3;

imdLoop::~imdLoop()
	{
	}

imdLoop::imdLoop(const char * host,int p) 
	{
	pthread_mutex_t mutextmp1 = PTHREAD_MUTEX_INITIALIZER;
	coordmutex=mutextmp1;

	pthread_mutex_t mutextmp2 = PTHREAD_MUTEX_INITIALIZER;
	forcemutex=mutextmp2;

	timesleep=0;
	strcpy(hostname,host);
	port = p;
	
	wait   = 1;                     // Connection configuration
	IMDmsg = 0;
	mode =0;//client
	pause  = false;

	nb_coords = 0;

	cont   = 1;                     // Main loop control
	ffreq  = 1;

	coords=NULL;
	forces_list=NULL;
	forces_atoms_list=NULL;
	nb_forces=0;
	}

void imdLoop::start()
	{
	int rc;
	rc = pthread_create(&imdthreads, NULL, run, (void *) this);
	if (rc)
		{
		exit(-1);
		}
	}



void imdLoop::init( imdLoop * imdl)
	{
	while ( ! IIMD_probeconnection()) 
		{
		imdl->MYIMDlog =  IIMD_init( imdl->hostname, &(imdl->mode),&(imdl->wait),&(imdl->port), &(imdl->IMDmsg),0 );
		usleep(500000);
		};

	imdl->timestep=0;
	}

void imdLoop::end( imdLoop * imdl)
	{
	IIMD_terminate ( );
	}

int imdLoop::iterate( imdLoop * imdl)
	{
	unsigned ret=0;
	//fprintf(imdl->MYIMDlog, "MYMDD >timestep=%d -\n",imdl->timestep);
	IIMD_treatprotocol();		
	if( !imdl->pause ) 
		{		
		pthread_mutex_lock( &(imdl->coordmutex) );
		ret= IIMD_get_coords( &(imdl->nb_coords)  ,  (float **) &(imdl->coords)) ;
		pthread_mutex_unlock( &(imdl->coordmutex) );
		if (ret) 
			{
			fprintf(imdl->MYIMDlog, 
					"MYMDD > \n");
			fprintf(imdl->MYIMDlog, "MYMDD > Send %d atoms (Time step=%d)\n", 
					imdl->nb_coords, imdl->timestep);
			fprintf(imdl->MYIMDlog, "MYMDD > ================================\n");
			fprintf(imdl->MYIMDlog, 
					"MYMDD > \n");
			fprintf(imdl->MYIMDlog,"MYMDD >  Force list (10 first atoms and the last one)\n");
			fprintf(imdl->MYIMDlog,
					"MYMDD > AtomID      x        y        z      \n");
			fprintf(imdl->MYIMDlog,
					"MYMDD > -------------------------------------\n");

			fprintf(imdl->MYIMDlog, "MYMDD > BKPT 1 -\n");
			}

		IIMD_treatprotocol();

		// New energies
		if ( IIMD_get_energies( &imdl->p_energies ) ) 
			{				
			fprintf(imdl->MYIMDlog, "MYMDD > \n");
			fprintf(imdl->MYIMDlog, "MYMDD > Send energies (Time step=%d)\n" , imdl->timestep);
			fprintf(imdl->MYIMDlog, "MYMDD > ================================\n");
			fprintf(imdl->MYIMDlog, "MYMDD >   \n");
			fprintf(imdl->MYIMDlog, "MYMDD >   MYPROGRAM Energy List (%d) \n", 99 );
			fprintf(imdl->MYIMDlog, 
					"MYMDD >   [Cal] for energies, [K] for the temperature \n" );
			fprintf(imdl->MYIMDlog, 
					"MYMDD >   [Bar] for pressure\n" );
			fprintf(imdl->MYIMDlog, 
					"MYMDD >   ------------------------------------------ \n");
			fprintf(imdl->MYIMDlog, "MYMDD >  \n");
			fprintf(imdl->MYIMDlog, "MYMDD >   VMD Energy List \n" );
			fprintf(imdl->MYIMDlog, "MYMDD >   --------------------\n");
			fprintf(imdl->MYIMDlog, "MYMDD >   Time step         [ ]   %12d\n", 
					imdl->p_energies->tstep);
			fprintf(imdl->MYIMDlog, "MYMDD >   Temperature       [K]   %12.5e\n", 
					imdl->p_energies->T);
			fprintf(imdl->MYIMDlog, "MYMDD >   Total E.          [Cal] %12.5e\n", 
					imdl->p_energies->Etot);
			fprintf(imdl->MYIMDlog, "MYMDD >   Bond E.           [Cal] %12.5e\n", 
					imdl->p_energies->Ebond);
			fprintf(imdl->MYIMDlog, "MYMDD >   Angle E.          [Cal] %12.5e\n", 
					imdl->p_energies->Eangle);
			fprintf(imdl->MYIMDlog, "MYMDD >   Potential E.      [Cal] %12.5e\n", 
					imdl->p_energies->Epot);
			fprintf(imdl->MYIMDlog, "MYMDD >   Dihedrale E.      [Cal] %12.5e\n", 
					imdl->p_energies->Edihe);
			fprintf(imdl->MYIMDlog, "MYMDD >   Improp. Dihed. E. [Cal] %12.5e\n", 
					imdl->p_energies->Eimpr);
			fprintf(imdl->MYIMDlog, "MYMDD >   Van der Waals E.  [Cal] %12.5e\n", 
					imdl->p_energies->Evdw);
			fprintf(imdl->MYIMDlog, "MYMDD >   Electrostatic. E. [Cal] %12.5e\n", 
					imdl->p_energies->Eelec);
			fprintf(imdl->MYIMDlog, "MYMDD > \n");
			}

		// Send forces every "ffreq"
		if ( (imdl->timestep % imdl->ffreq) == 0 )  
			{
			imdl->updateForces();
			pthread_mutex_lock( &(imdl->forcemutex) );		
			if(imdl->nb_forces!=0)	
				IIMD_send_forces( &imdl->nb_forces,  imdl->forces_atoms_list,  (const float *) imdl->forces_list ); 						
			pthread_mutex_unlock( &(imdl->forcemutex) );		
			}

		}
	usleep(imdl->timesleep);
	imdl->timestep++;
	return ret;
	}


int imdLoop::getCoords(float * coordinates)
	{
	if(coords!=NULL && nb_coords>0)
		{
		pthread_mutex_lock( &coordmutex );
		memcpy(coordinates,coords,nb_coords*sizeof(float)*3);
		pthread_mutex_unlock( &coordmutex );
		return nb_coords;
		}
	else
		{
		return -1;
		}
	}

int imdLoop::getCoords(float * coordinates,int atomsnumber)
	{
	if(coords!=NULL && atomsnumber<=nb_coords)
		{
		pthread_mutex_lock( &coordmutex );
		memcpy(coordinates,coords,atomsnumber*sizeof(float)*3);
		/*unsigned k=0;
		float x,y,z;	
		for(unsigned i=0;i<atomsnumber;i++)
			{
			cout<<coords[k++]<<" "<<coords[k++]<<" "<<coords[k++]<<endl;
			}*/
		pthread_mutex_unlock( &coordmutex );		
		return atomsnumber;
		}
	else
		{
		return -1;
		}
	}

int imdLoop::setForces(int nbforce,int * atomslist, float * forceslist)
	{
	pthread_mutex_lock( &forcemutex );

	if(this->forces_list!=NULL)
		{
		delete[] this->forces_list;
		this->forces_list=NULL;
		}

	if(this->forces_atoms_list!=NULL)
		{
		delete[] this->forces_atoms_list;
		this->forces_atoms_list=NULL;		
		}
	this->nb_forces=nbforce;	
	this->forces_list=new float[3*nbforce];
	this->forces_atoms_list=new int[nbforce];
	memcpy(this->forces_list,forceslist,nbforce*sizeof(float)*3);
	memcpy(this->forces_atoms_list,atomslist,nbforce*sizeof(int));
	pthread_mutex_unlock( &forcemutex );
	return nbforce;
	}

void imdLoop::setTimeSleep(int usleep)
	{
	this->timesleep=usleep;
	}


void  * imdLoop::run(void * userdata)
	{
	imdLoop * imdl=(imdLoop *)userdata;
	init(imdl);	
	while ( imdl->cont ) 
		{
		iterate(imdl);
		}	
	end(imdl);	
	}

void imdLoop::updateForces()
	{
	}
