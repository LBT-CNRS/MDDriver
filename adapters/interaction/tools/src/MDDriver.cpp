#include "MDDriver.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;
MDDriver::MDDriver(const char * hostname,int port) : imdLoop(hostname,port)
	{
	}

MDDriver::~MDDriver()
	{
	}


void MDDriver::getForce(int id, float * x, float  * y,float * z)
	{
	*x=0.0;
	*y=0.0;
	*z=0.0;
	
	int findid=findForceId(id);
	if(findid!=-1)
		{
		*x=forces_list[findid*3];
		*y=forces_list[findid*3+1];
		*z=forces_list[findid*3+2];
		}
	}


void MDDriver::addForce(int id, float x, float y,float z)
	{
	if(id<nb_coords)
		{
		if(nb_forces==0)
			{
			nb_forces=1;
			forces_list=new float[3];
			forces_atoms_list=new int[1];
			forces_atoms_list[0]=id;
			forces_list[0]=x;
			forces_list[1]=y;
			forces_list[2]=z;
			}
		else
			{
			int findid=findForceId(id);
			int nbforce=nb_forces;
			if(findid==-1)
				{

				float * forces=new float[(nbforce+1)*3];
				int * forceids=new int [nbforce+1];
				
	
				memcpy(forces,forces_list,sizeof(float)*nbforce*3);
				memcpy(forceids,forces_atoms_list,sizeof(int)*nbforce);
				
				removeForces();

				forces_list=forces;
				forces_atoms_list=forceids;
				forces_atoms_list[nbforce]=id;
				forces_list[(nbforce)*3]=x;
				forces_list[(nbforce)*3+1]=y;
				forces_list[(nbforce)*3+2]=z;
				nb_forces=nbforce+1;
				
				}
			else
				{
				forces_list[(findid)*3]+=x;
				forces_list[(findid)*3+1]+=y;
				forces_list[(findid)*3+2]+=z;
				}
	
			}
		}	
	}

void MDDriver::setForce(int id, float x, float y,float z)
	{
	if(id<nb_coords)
		{
		if(nb_forces==0)
			{
			nb_forces=1;		
			forces_list=new float[3];
			forces_atoms_list=new int[1];
			forces_atoms_list[0]=id;
			forces_list[0]=x;
			forces_list[1]=y;
			forces_list[2]=z;
			}
		else
			{
			int findid=findForceId(id);
			int nbforce=nb_forces;
			if(findid==-1)
				{
				float * forces=new float[(nbforce+1)*3];
				int * forceids=new int [nbforce+1];
					
				memcpy(forces,forces_list,sizeof(float)*nbforce*3);
				memcpy(forceids,forces_atoms_list,sizeof(int)*nbforce);
				
				removeForces();

				forces_list=forces;
				forces_atoms_list=forceids;
				forces_atoms_list[nbforce]=id;
				forces_list[(nbforce)*3]=x;
				forces_list[(nbforce)*3+1]=y;
				forces_list[(nbforce)*3+2]=z;
				nb_forces=nbforce+1;
				
				}
			else
				{
				forces_list[(findid)*3]=x;
				forces_list[(findid)*3+1]=y;
				forces_list[(findid)*3+2]=z;
				}
	
			}
		}	
	}

void MDDriver::getCoord(int id,float *x, float *y,float *z)
	{
	if(id<nb_coords)
		{	
		*x=forces_list[id*3];
		*y=forces_list[id*3+1];
		*z=forces_list[id*3+2];
		}
	}


int MDDriver::findForceId(int id)
	{
	for(unsigned i=0;i<nb_forces;i++)
		{
		if(id==forces_atoms_list[i])
			return i;
		}
	return -1;
	}	


void MDDriver::removeForces()
	{
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
	nb_forces=0;
	}


void MDDriver::removeForce(int id)
	{	
	unsigned indexfinded=-1;
	
	if(nb_forces==1)
		{
		removeForces();	
		}
	else
		{
		indexfinded=findForceId(id);
		if(indexfinded!=-1)
			{
			float * forces=new float [(nb_forces-1)*3];
			int * forceids=new int [nb_forces-1];

			forces_atoms_list[indexfinded]=forces_atoms_list[nb_forces-1];
			forces_list[indexfinded*3]=forces_list[(nb_forces-1)*3];
			forces_list[indexfinded*3+1]=forces_list[(nb_forces-1)*3+1];
			forces_list[indexfinded*3+2]=forces_list[(nb_forces-1)*3+2];

			memcpy(forces,forces_list,sizeof(float)*(nb_forces-1)*3);
			memcpy(forceids,forces_atoms_list,sizeof(int)*nb_forces-1);
			removeForces();	

			forces_atoms_list=forceids;
			forces_list=forces;
			nb_forces--;
			}
		}
	}



