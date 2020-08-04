#include "MDDriver.h"

void MDDriver::updateForces()
	{
	removeForces();
	setForce(0,1.0,1.0,1.0);
	setForce(0,-1.0,-1.0,1.0);
	setForce(1,0.0,-1.0,2.0);
	}
