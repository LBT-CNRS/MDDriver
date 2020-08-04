#include "Python.h"

#include "imd_interface.h"

PyDoc_STRVAR(mddriver__doc__,
			 "Python interface to MDDriver");


PyDoc_STRVAR(mddriver_client__doc__,
			 "Must be call before by the client before starting a interactive molecular dynamics. ");

PyDoc_STRVAR(mddriver_server__doc__,
			 "Must be call before by the server (simulation code) and by the client before starting a interactive molecular dynamics. ");

PyDoc_STRVAR(mddriver_probeconnection__doc__, "mddriver_probeconnection__doc__");
PyDoc_STRVAR(mddriver_treatprotocol__doc__, "mddriver_treatprotocol__doc__");
PyDoc_STRVAR(mddriver_senddisconnect__doc__, "mddriver_senddisconnect__doc__");
PyDoc_STRVAR(mddriver_sendpause__doc__, "mddriver_sendpause__doc__");
PyDoc_STRVAR(mddriver_sendkill__doc__, "mddriver_sendkill__doc__");
PyDoc_STRVAR(mddriver_terminate__doc__, "mddriver_terminate__doc__");

PyDoc_STRVAR(mddriver_sendcoords__doc__, "mddriver_sendcoords__doc__");
PyDoc_STRVAR(mddriver_sendcurrentenergies__doc__, "mddriver_sendcurrentenergies__doc__");
PyDoc_STRVAR(mddriver_sendstepsenergies__doc__, "mddriver_sendstepsenergies__doc__");
PyDoc_STRVAR(mddriver_getforces__doc__, "mddriver_getforces__doc__");

PyDoc_STRVAR(mddriver_sendtrate__doc__, "mddriver_sendtrate__doc__");
PyDoc_STRVAR(mddriver_sendforces__doc__, "mddriver_sendforces__doc__");
PyDoc_STRVAR(mddriver_getcoords__doc__, "mddriver_getcoords__doc__");
PyDoc_STRVAR(mddriver_getenergies__doc__, "mddriver_getenergies__doc__");

static PyObject * mddriver_client(PyObject *self, PyObject *args)
	{
	char * filename;
	int mode=0;
	int wait=0;
	int port=3000;
	int message=0;
	char * hostname;
	if (!PyArg_ParseTuple(args, "siis:mddriver_client", &hostname, &port, &message, &filename))
		{
		return NULL;
		}
	IIMD_init(hostname, &mode, &wait, &port,  &message, filename );	
	Py_INCREF(Py_None);
	return Py_None;
	}

static PyObject * mddriver_server(PyObject *self, PyObject *args)
	{
	
	char * filename;
	int mode=1;
	int wait=0;
	int port=3000;
	int message=0;
	if (!PyArg_ParseTuple(args, "iiis:mddriver_server",&port,&wait,&message,&filename))
		{
		return NULL;
		}
	IIMD_init("", &mode, &wait, &port,  &message, filename );	
	Py_INCREF(Py_None);
	return Py_None;
	}


static PyObject * mddriver_probeconnection(PyObject *self, PyObject *args)
	{
	return  Py_BuildValue("i", IIMD_probeconnection());
	}


static PyObject * mddriver_treatprotocol(PyObject *self, PyObject *args)
	{
	IIMD_treatprotocol     ( );	
	Py_INCREF(Py_None);
	return Py_None;
	}


static PyObject * mddriver_senddisconnect(PyObject *self, PyObject *args)
	{
	IIMD_send_disconnect     ( );
	Py_INCREF(Py_None);
	return Py_None;
	}


static PyObject * mddriver_sendpause(PyObject *self, PyObject *args)
	{
	IIMD_send_pause        ( );
	Py_INCREF(Py_None);
	return Py_None;
	}

static PyObject * mddriver_sendkill(PyObject *self, PyObject *args)	
	{
	IIMD_send_kill        ( );
	Py_INCREF(Py_None);
	return Py_None;
	}

static PyObject * mddriver_terminate(PyObject *self, PyObject *args)
	{
	IIMD_terminate         ( );
	Py_INCREF(Py_None);
	return Py_None;
	}

static PyObject * mddriver_getforces(PyObject *self, PyObject *args)
	{
	int nbatoms;
	imd_int32 * atomids; 
	float * atomforces; 		
	IIMD_get_forces        ( &nbatoms, &atomids, &atomforces);	
	PyObject* dict=PyDict_New();
	if (!dict)
		return NULL;
	
	unsigned i=0;
	for(i=0;i<nbatoms;i++)
		{
		PyObject * ids=Py_BuildValue("i", atomids[i]);
		PyObject * forces=Py_BuildValue("(fff)", atomforces[i*3], atomforces[i*3+1], atomforces[i*3+2]);
		PyDict_SetItem(dict, ids,forces);
		}
	return  dict;
	}

static PyObject * mddriver_sendcoords(PyObject *self, PyObject *args)
	{
	PyObject * poslist=NULL;
	if (!PyArg_ParseTuple(args, "O:mddriver_sendcoords",&poslist))
		{
		return NULL;
		}
	else 
		{
		int i, n; 
		PyObject *item; 
		n = PyList_Size(poslist); 
				
		if (n < 0)
			return NULL;; /* Not a list */ 
		float * posatoms=(float *) malloc (n*sizeof(float)*3);
		memset(posatoms, 0, n*sizeof(float)*3);
		for (i = 0; i < n; i++) 
			{
			item = PyList_GetItem(poslist, i); /* Can’t fail */ 
			if (!PyTuple_Check(item)) 
				{
				printf("Erreur");
				continue;
				}
			if(!PyTuple_Size(item)==3)
				{
				printf("Erreur");
				continue;
				}
			posatoms[i*3]  =  PyFloat_AsDouble(PyTuple_GetItem(item, 0));
			posatoms[i*3+1]=  PyFloat_AsDouble(PyTuple_GetItem(item, 1));
			posatoms[i*3+2]=  PyFloat_AsDouble(PyTuple_GetItem(item, 2));
			}
		IIMD_send_coords( &n , posatoms );
		
		free(posatoms);
		Py_INCREF(Py_None);
		return Py_None;
		}
	}

static PyObject * mddriver_sendcurrentenergies(PyObject *self, PyObject *args)
	{
	PyObject * energiesdict=NULL;
	if (!PyArg_ParseTuple(args, "O:mddriver_sendcurrentenergies",&energiesdict))
		{
		return NULL;
		}
	
	if (!PyDict_Check(energiesdict))
		{
		printf("Erreur");
		return NULL;
		}	
	
	IMDEnergies energies;

	memset(&energies,0, sizeof(energies) );
	
	PyObject * energy=NULL;
	PyObject * enertype=NULL;
	
	Py_ssize_t it=0;
	unsigned i=0;
	
	while(PyDict_Next(energiesdict, &it, &enertype, &energy))
		{
		if (!PyString_Check(enertype))
			{
			return NULL;
			}
		char * typestr=NULL;
		typestr=PyString_AsString(enertype);
		if (strcmp(typestr, "tstep")==0)
			{
			if (!PyInt_Check(energy))
				{
				return NULL;
				}	
			energies.tstep  = PyInt_AsLong(energy);
			}	

		if (strcmp(typestr, "T")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.T      = PyFloat_AsDouble(energy);
			}	

		if (strcmp(typestr, "Etot")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.Etot      = PyFloat_AsDouble(energy);
			}	

		if (strcmp(typestr, "Epot")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.Epot      = PyFloat_AsDouble(energy);
			}	

		if (strcmp(typestr, "Evdw")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.Evdw      = PyFloat_AsDouble(energy);
			}	

		if (strcmp(typestr, "Eelec")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.Eelec      = PyFloat_AsDouble(energy);
			}	

		if (strcmp(typestr, "Eangle")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.Eangle      = PyFloat_AsDouble(energy);
			}	

		if (strcmp(typestr, "Edihe")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.Edihe      = PyFloat_AsDouble(energy);
			}	
		if (strcmp(typestr, "Eimpr")==0)
			{
			if (!PyFloat_Check(energy))
				{
				return NULL;
				}	
			energies.Eimpr      = PyFloat_AsDouble(energy);
			}
		i++;
		}
	IIMD_send_energies( &energies );
	Py_INCREF(Py_None);
	return Py_None;
	}

/*TODO*/
static PyObject * mddriver_sendstepsenergies(PyObject *self, PyObject *args)
	{
	//IIMD_send_energy_array ( const imd_int32   *tstep, const IMDEnergies *energies );
	Py_INCREF(Py_None);
	return Py_None;
	}


static PyObject * mddriver_sendtrate(PyObject *self, PyObject *args)
	{
	imd_int32 nbstep;
	if (!PyArg_ParseTuple(args, "i:mddriver_sendtrate", &nbstep))
		{
		return NULL;
		}
	IIMD_send_trate( &nbstep);
	Py_INCREF(Py_None);
	return Py_None;
	}	

static PyObject * mddriver_getcoords(PyObject *self, PyObject *args)
	{
	imd_int32 nbatoms=0;
	float * posatoms=NULL;	
	
	IIMD_get_coords( &nbatoms, &posatoms);
	printf("nb atoms %d\n", nbatoms);
	PyObject *lst = PyList_New(nbatoms);
	if (!lst)
		{
		return NULL;
		}
	unsigned i=0;
	for (i = 0; i < nbatoms; i++) 
		{
		PyObject * pos=Py_BuildValue("(fff)", posatoms[i*3],posatoms[i*3+1],posatoms[i*3+2] );

		PyList_SetItem(lst, i, pos);
		}
	return lst;
		}	


static PyObject * mddriver_sendforces(PyObject *self, PyObject *args)
	{
	PyObject * forcesdict=NULL;
	if (!PyArg_ParseTuple(args, "O:mddriver_sendforces",&forcesdict))
		{
		return NULL;
		}
	
	if (!PyDict_Check(forcesdict))
		{
		printf("Erreur");
		return NULL;
		}	
	
	int nbforces = PyDict_Size(forcesdict); 

	if (nbforces < 0)
		{
		printf("Erreur");			
		return NULL;; /* Not a Dict */ 
		}
	float * forceatoms=(float *) malloc (nbforces*sizeof(float)*3);
	int * forceatomsids=(int *) malloc (nbforces*sizeof(int));		
	memset(forceatoms, 0, nbforces*sizeof(float)*3);
	memset(forceatomsids, 0, nbforces*sizeof(int));		
	
	PyObject * force=NULL;
	PyObject * id=NULL;
	
	Py_ssize_t it=0;
	unsigned i=0;
	while(PyDict_Next(forcesdict, &it, &id, &force))
		{
		if (!PyTuple_Check(force)) 
			{
			printf("Erreur");
			return NULL;
			}
		if (!PyInt_Check(id)) 
			{
			printf("Erreur");
			return NULL;
			}
		forceatoms[i*3]  =  PyFloat_AsDouble(PyTuple_GetItem(force, 0));
		forceatoms[i*3+1]=  PyFloat_AsDouble(PyTuple_GetItem(force, 1));
		forceatoms[i*3+2]=  PyFloat_AsDouble(PyTuple_GetItem(force, 2));
		forceatomsids[i]= PyInt_AsLong(id);
		i++;
		}
	IIMD_send_forces(&nbforces , forceatomsids,  forceatoms);
	
	free(forceatoms);
	free(forceatomsids);
	
	Py_INCREF(Py_None);
	return Py_None;
	}


/*TODO*/
static PyObject * mddriver_getenergies(PyObject *self, PyObject *args)
	{
	IMDEnergies * energies;
	if(	IIMD_get_energies      ( &energies))
		{
		PyObject* dict=PyDict_New();
		if (!dict)
			return NULL;

	
		PyDict_SetItem(dict, PyString_FromString("tstep"),PyInt_FromLong(energies->tstep));
		PyDict_SetItem(dict, PyString_FromString("T"),PyFloat_FromDouble(energies->T));
		PyDict_SetItem(dict, PyString_FromString("Etot"),PyFloat_FromDouble(energies->Etot));
		PyDict_SetItem(dict, PyString_FromString("Epot"),PyFloat_FromDouble(energies->Epot));
		PyDict_SetItem(dict, PyString_FromString("Evdw"),PyFloat_FromDouble(energies->Evdw));
		PyDict_SetItem(dict, PyString_FromString("Eelec"),PyFloat_FromDouble(energies->Eelec));
		PyDict_SetItem(dict, PyString_FromString("Eangle"),PyFloat_FromDouble(energies->Eangle));
		PyDict_SetItem(dict, PyString_FromString("Edihe"),PyFloat_FromDouble(energies->Edihe));
		PyDict_SetItem(dict, PyString_FromString("Eimpr"),PyFloat_FromDouble(energies->Eimpr));
		return  dict;
		}
	else
		{
		Py_INCREF(Py_None);
		return Py_None;
		}
	
	}



/*
PyObject *lst = PyList_New(array_len);
if (!lst)
return NULL;
for (i = 0; i < array_len; i++) {
    PyObject *num = PyFloat_FromDouble(array[i]);
    if (!num) {
        Py_DECREF(lst);
        return NULL;
    }
    PyList_SET_ITEM(lst, i, num);   // reference to num stolen
}
return lst;
*/


/* A list of all the methods defined by this module. */
/* "testmddriver" is the name seen inside of Python */
/* "testmddriver" is the name of the C function handling the Python call */
/* "METH_VARGS" tells Python how to call the handler */
/* The {NULL, NULL} entry indicates the end of the method definitions */
static PyMethodDef mddriver_methods[] = {
	{"mddriver_server",  mddriver_server, METH_VARARGS, mddriver_server__doc__},
	{"mddriver_client",  mddriver_client, METH_VARARGS, mddriver_client__doc__},

	{"mddriver_probeconnection",  mddriver_probeconnection, METH_VARARGS, mddriver_probeconnection__doc__},
	{"mddriver_treatprotocol",  mddriver_treatprotocol, METH_VARARGS, mddriver_treatprotocol__doc__},
	{"mddriver_senddisconnect",  mddriver_senddisconnect, METH_VARARGS, mddriver_senddisconnect__doc__},
	{"mddriver_sendpause",  mddriver_sendpause, METH_VARARGS, mddriver_sendpause__doc__},
	{"mddriver_sendkill",  mddriver_sendkill, METH_VARARGS, mddriver_sendkill__doc__},
	{"mddriver_terminate",  mddriver_terminate, METH_VARARGS, mddriver_terminate__doc__},

	{"mddriver_sendcurrentenergies",  mddriver_sendcurrentenergies, METH_VARARGS, mddriver_sendcurrentenergies__doc__},
	{"mddriver_sendstepsenergies",  mddriver_sendstepsenergies, METH_VARARGS, mddriver_sendstepsenergies__doc__},
	{"mddriver_getforces",  mddriver_getforces, METH_VARARGS, mddriver_getforces__doc__},
	{"mddriver_sendcoords",  mddriver_sendcoords, METH_VARARGS, mddriver_sendcoords__doc__},

	{"mddriver_sendforces",  mddriver_sendforces, METH_VARARGS, mddriver_sendforces__doc__},
	{"mddriver_getcoords",  mddriver_getcoords, METH_VARARGS, mddriver_getcoords__doc__},	
	{"mddriver_sendtrate",  mddriver_sendtrate, METH_VARARGS, mddriver_sendtrate__doc__},
	{"mddriver_getenergies",  mddriver_getenergies, METH_VARARGS, mddriver_getenergies__doc__},	
	
	
	{NULL, NULL}      /* sentinel */
};




/* When Python imports a C module named 'X' it loads the module */
/* then looks for a method named "init"+X and calls it.  Hence */
/* for the module "mandelbrot" the initialization function is */
/* "initmddriver".  The PyMODINIT_FUNC helps with portability */
/* across operating systems and between C and C++ compilers */

PyMODINIT_FUNC
initmddriver(void)
{
	printf("Importing MDDriver\n");
	/* There have been several InitModule functions over time */
	Py_InitModule3("mddriver", mddriver_methods,
                   mddriver__doc__);
}