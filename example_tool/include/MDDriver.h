#ifndef _MDDRIVER_H_
#define _MDDRIVER_H_

#include <stdio.h>
#include <stdlib.h>
#include "imd_interface.h"
#include "imdLoop.h"

#define HOSTNAMESTRINGSIZE 256 

class MDDriver : public imdLoop
	{
	public : 
		
		MDDriver(const char * hostname,int port);
		virtual ~MDDriver();

		void getCoord(int id,float *x, float *y,float *z);

		virtual void updateForces();
		void removeForces();

		void addForce(int id, float x, float y,float z);
		void setForce(int id, float x, float y,float z);
		void getForce(int id,float *x, float *y,float *z);

		void removeForce(int id);
		int findForceId(int id);


	};

#endif
