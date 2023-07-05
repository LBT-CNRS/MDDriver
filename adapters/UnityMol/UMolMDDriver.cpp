///First compile MDDriver to obtain the libmddriver.a
/// For windows MinGW: g++ -fPIC --shared -O2 UMolMDDriver.c -I ../../protocol/include/ -I ../interaction/tools/include/ -lmddriver -L ../mddinstall/ -lws2_32 -o Unity_MDDriver.dll


#include "imd.h"
#include "imd_interface.h"

//#include "MDDriver.h"


#include <stdbool.h>
#include <string.h>
//#include <iostream>
#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#include <stdlib.h>
#endif


#if defined(__unix__) || defined(__linux__) || defined(__APPLE__) || defined(__MACH__)
#define OS_UNIX
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define OS_OSX
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(__CYGWIN__)
#define OS_WINDOWS
#endif

//
// API export macro
//
#if defined(OS_OSX)
#define API __attribute__((visibility("default")))
#elif defined(OS_WINDOWS)
#define API __declspec(dllexport)
#else
#define API
#endif


typedef float t_coord[3];


class MDDriverAdapter {
public:
    int N_atoms;
    t_coord *coords;
    FILE *MYIMDlog;
    int nb_forces ;
    float *forces_list ;
    int *forces_atoms_list ;

    int N_floats;
    float *customfloat;
    char * datanamefloat;

    int N_ints;
    int *customint;
    char * datanameint;

    MDDriverAdapter();
};

MDDriverAdapter::MDDriverAdapter()
{
  N_atoms = 0;
  coords=NULL;
  MYIMDlog = NULL;

  nb_forces = 0;
  forces_list = NULL;
  forces_atoms_list = NULL;

  N_floats = 0;
  customfloat = NULL;
  datanamefloat=(char *) malloc(sizeof(char)*8);
  N_ints = 0;
  customint= NULL;
  datanameint=(char *) malloc(sizeof(char)*8);

}

void MDDriver_freeForces(MDDriverAdapter *mddinstance);

#ifdef __cplusplus
extern "C"
{
#endif
class MDDriverAdapter;
API MDDriverAdapter *createMDDriverInstance();
API void deleteMDDriverInstance(MDDriverAdapter *mddinstance);
API int MDDriver_init(MDDriverAdapter *mddinstance, const char *hostname, int port);
API int MDDriver_start(MDDriverAdapter *mddinstance);
API int MDDriver_stop(MDDriverAdapter *mddinstance);
API bool MDDriver_isConnected(MDDriverAdapter *mddinstance);
API int MDDriver_getNbParticles(MDDriverAdapter *mddinstance);
API int MDDriver_getNbCustomFloat(MDDriverAdapter *mddinstance);
API int MDDriver_getNbCustomInt(MDDriverAdapter *mddinstance);
API int MDDriver_getPositions(MDDriverAdapter *mddinstance, float *verts, int nbParticles);
API int MDDriver_getCustomFloat(MDDriverAdapter *mddinstance, char * datanamefloat, float *customfloat, int nbfloat);
API int MDDriver_getCustomInt(MDDriverAdapter *mddinstance, char * datanameint, int * customint, int nbint);
API void MDDriver_pause(MDDriverAdapter *mddinstance);
API void MDDriver_play(MDDriverAdapter *mddinstance);
API void MDDriver_setForces(MDDriverAdapter *mddinstance, int nbforces, int *atomslist, float *forceslist);
API void MDDriver_getEnergies(MDDriverAdapter *mddinstance, IMDEnergies *energies);
API void MDDriver_getGrid(MDDriverAdapter *mddinstance, IMDGrid *grid);
API int MDDriver_loop(MDDriverAdapter *mddinstance);
API void MDDriver_disconnect(MDDriverAdapter *mddinstance);

#ifdef __cplusplus
}
#endif

MDDriverAdapter *createMDDriverInstance() {
    return new MDDriverAdapter();
}

void deleteMDDriverInstance(MDDriverAdapter *mddinstance) {
    IIMD_terminate();
    mddinstance->N_atoms = 0;
    MDDriver_freeForces(mddinstance);
    delete mddinstance;
}

int MDDriver_init(MDDriverAdapter *mddinstance, const char *hostname, int port) {
    int mode = 0;
    int wait = 0;
    int IMDmsg = 0;
    // int IMDmsg = 3;
    int IMDPort = port;
    mddinstance->N_atoms = 0;
    int tries = 5;//5*500ms = 2.5 seconds
    int t = 0;

    while (!IIMD_probeconnection()) {
        IIMD_init( hostname, &mode, &wait, &IMDPort, &IMDmsg , 0);
        // IIMD_init( hostname, &mode, &wait, &IMDPort, &IMDmsg , "MDDriverDebug.txt");
        if (IIMD_probeconnection())
            return 0;
        IIMD_treatevent();
#if defined(_WIN32)
        Sleep(500);
#else
        usleep(500000);
#endif
        t++;
        if (t >= tries)
        {
            return 1;
        }
    }
    return 0;
}

int MDDriver_start(MDDriverAdapter *mddinstance)
{
    // int rc;
    // rc = pthread_create(&imdthreads, NULL, run, (void *) this);
    // if (rc)
    //     return 0;
    // return 1;
    return 1;

}

void MDDriver_freeForces(MDDriverAdapter *mddinstance) {
    if (mddinstance->forces_list != NULL) {
        delete mddinstance->forces_list; //free(mddinstance->forces_list);
        delete mddinstance->forces_atoms_list; //free(mddinstance->forces_atoms_list);
        mddinstance->forces_list = NULL;
        mddinstance->forces_atoms_list = NULL;
    }
    mddinstance->nb_forces = 0;
}

int MDDriver_stop(MDDriverAdapter *mddinstance) {
    if (IIMD_probeconnection()) {
        IIMD_send_kill();
        IIMD_send_disconnect();
    }
    // IIMD_terminate ();
    mddinstance->N_atoms = 0;
    MDDriver_freeForces(mddinstance);
    return 1;
}

void MDDriver_disconnect(MDDriverAdapter *mddinstance) {
    if (IIMD_probeconnection()) {
        IIMD_send_disconnect();
    }
}


bool MDDriver_isConnected(MDDriverAdapter *mddinstance) {
    return IIMD_probeconnection() == 1;
}

int MDDriver_getNbParticles(MDDriverAdapter *mddinstance) {
    if (mddinstance->N_atoms <= 0) {
        IIMD_get_coords( &(mddinstance->N_atoms), (float **) & (mddinstance->coords) );
    }
    return mddinstance->N_atoms;
}

int MDDriver_getNbCustomFloat(MDDriverAdapter *mddinstance) {
    if (mddinstance->N_floats <= 0) {
        IIMD_get_custom_float( &(mddinstance->datanamefloat), &(mddinstance->N_floats), (float **) & (mddinstance->customfloat) );
    }
    return mddinstance->N_floats;
}

int MDDriver_getNbCustomInt(MDDriverAdapter *mddinstance) {
    if (mddinstance->N_ints <= 0) {
        IIMD_get_custom_int( &(mddinstance->datanameint), &(mddinstance->N_ints), (int **) & (mddinstance->customint) );
    }
    return mddinstance->N_ints;
}


void MDDriver_pause(MDDriverAdapter *mddinstance) {
    IIMD_send_pause();
}

void MDDriver_play(MDDriverAdapter *mddinstance) {
    IIMD_send_pause();
}

int MDDriver_getPositions(MDDriverAdapter *mddinstance, float *coordinates, int nbPos) {
    IIMD_get_coords( &(mddinstance->N_atoms), (float **) & (mddinstance->coords) );
    memcpy(coordinates, mddinstance->coords, nbPos * sizeof(float) * 3);
    return nbPos;
}


int MDDriver_getCustomFloat(MDDriverAdapter *mddinstance, char * datanamefloat, float *customfloat, int nbfloat) {
    IIMD_get_custom_float(&(mddinstance->datanamefloat),  &(mddinstance->N_floats), (float **) & (mddinstance->customfloat) );
    memcpy(customfloat, mddinstance->customfloat, nbfloat * sizeof(float));
    memcpy(datanamefloat, mddinstance->datanamefloat, 8 * sizeof(char));
    return nbfloat;
}

int MDDriver_getCustomInt(MDDriverAdapter *mddinstance, char * datanameint, int * customint, int nbint) {
  IIMD_get_custom_float(&(mddinstance->datanameint) , &(mddinstance->N_ints), (float **) & (mddinstance->customint) );
  memcpy(customint, mddinstance->customint, nbint * sizeof(int));
  memcpy(datanameint, mddinstance->datanameint, 8 * sizeof(char));
  return nbint;
}



void MDDriver_setForces(MDDriverAdapter *mddinstance, int nbforces, int *atomslist, float * forceslist) {
    if (mddinstance->nb_forces != nbforces || mddinstance->forces_list == NULL) {
        if (mddinstance->forces_list != NULL) {
            delete mddinstance->forces_list;
            delete mddinstance->forces_atoms_list;
        }
        mddinstance->forces_list = new float[3 * nbforces];//(float *)malloc(sizeof(float) * 3 * nbforces);
        mddinstance->forces_atoms_list = new int[nbforces];//(int *)malloc(sizeof(int) * nbforces);
    }
//mettre les precedentes forces a 0 ?
    memcpy(mddinstance->forces_list, forceslist, nbforces * sizeof(float) * 3);
    memcpy(mddinstance->forces_atoms_list, atomslist, nbforces * sizeof(int));

    mddinstance->nb_forces = nbforces;
}

void MDDriver_getEnergies(MDDriverAdapter *mddinstance, IMDEnergies *energies) {
    IMDEnergies *curE;
    IIMD_get_energies( &curE);

    energies->tstep = curE->tstep;
    energies->T = curE->T;
    energies->Etot = curE->Etot;
    energies->Epot = curE->Epot;
    energies->Evdw = curE->Evdw;
    energies->Eelec = curE->Eelec;
    energies->Ebond = curE->Ebond;
    energies->Eangle = curE->Eangle;
    energies->Edihe = curE->Edihe;
    energies->Eimpr = curE->Eimpr;
}

void MDDriver_getGrid(MDDriverAdapter *mddinstance, IMDGrid *grid) {
    IMDGrid *curG;
    IIMD_get_grid( &curG);

    grid->tstep = curG->tstep;
    grid->Xorigin = curG->Xorigin;
    grid->Yorigin = curG->Yorigin;
    grid->Zorigin = curG->Zorigin;

    grid->Xend = curG->Xend;
    grid->Yend = curG->Yend;
    grid->Zend = curG->Zend;

    grid->XdirectionX = curG->XdirectionX;
    grid->YdirectionX = curG->YdirectionX;
    grid->ZdirectionX = curG->ZdirectionX;

    grid->XdirectionY = curG->XdirectionY;
    grid->YdirectionY = curG->YdirectionY;
    grid->ZdirectionY = curG->ZdirectionY;

    grid->XdirectionZ = curG->XdirectionZ;
    grid->YdirectionZ = curG->YdirectionZ;
    grid->ZdirectionZ = curG->ZdirectionZ;

    grid->nbcellx = curG->nbcellx;
    grid->nbcelly = curG->nbcelly;
    grid->nbcellz = curG->nbcellz;

    grid->sizecellx = curG->sizecellx;
    grid->sizecelly = curG->sizecelly;
    grid->sizecellz = curG->sizecellz;

}

int MDDriver_loop(MDDriverAdapter *mddinstance) {
    if (!IIMD_probeconnection())
        return 0;
    IIMD_treatprotocol();

    if (mddinstance->nb_forces != 0) {
        IIMD_send_forces( &(mddinstance->nb_forces), mddinstance->forces_atoms_list, mddinstance->forces_list);
    }
    return 1;
}
