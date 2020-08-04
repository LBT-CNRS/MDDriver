#include "MDDriver.h" 

#include <stdlib.h>
#include <iostream>


#define PDBNAMEMAXSIZE 256
#define HOSTMAXSIZE 256

using namespace std;

void usage()
	{
	cout<<"mddriverconstraint"<<endl;
	cout<<"\tOptions : "<<endl;
	cout<<"\t\t[-host hostname] : default = localhost"<<endl;
	cout<<"\t\t[-port port] : default = 3000"<<endl;
	exit(1);
	}

int main( int argc, char *argv[] )
	{
	unsigned port=3000;
	char hostname[HOSTMAXSIZE]="localhost";
	unsigned minimaloptionok=false;

	for(unsigned i=1;i<argc;i++)
		{
		if(strcmp("-help",argv[i])==0)
			{
			usage();
			}

		else if(strcmp("-host",argv[i])==0)
			{
			i++;
			if(i<argc)
				strcpy(hostname,argv[i]);
			else 	
				usage();
			}
		else if(strcmp("-port",argv[i])==0)
			{
			i++;
			if(i<argc)
				port=atoi(argv[i]);
			else 	
				usage();
			}


		}
	
	cout<<"MDDriver loop started"<<endl;
	MDDriver * imdl=new MDDriver(hostname,port);
	imdl->start();

	while(1)
		{
		usleep(500000);
		}
		

	return 0;
	}
