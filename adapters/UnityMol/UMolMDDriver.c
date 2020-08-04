///First compile MDDriver to obtain the libmddriver.a
/// For windows MinGW: g++ -fPIC --shared -O2 UMolMDDriver.c -I ../../protocol/include/ -I ../interaction/tools/include/ -lmddriver -L ../mddinstall/ -lws2_32 -o Unity_MDDriver.dll


#include "imd.h"
#include "imd_interface.h"

#include "MDDriver.h"

#include <iostream>
#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

extern void MDDriver_init(const char *hostname, int port);
extern int MDDriver_start();
extern int MDDriver_stop();
extern bool MDDriver_isConnected();
extern int MDDriver_probeconnection();
extern int MDDriver_getNbParticles();
extern int MDDriver_getPositions(float *verts, int nbParticles);
extern void MDDriver_pause();
extern void MDDriver_play();
extern void MDDriver_setForces(int nbforces, int *atomslist, float *forceslist);
extern void MDDriver_resetForces();
extern void MDDriver_getEnergies(IMDEnergies **energies);
extern void MDDriver_loop();
extern void MDDriver_disconnect();

#ifdef __cplusplus
}
#endif


typedef float t_coord[3];

// pthread_t imdthreads;
// static void  * run(void * userdata);
int N_atoms = 0;
t_coord *coords;
FILE *MYIMDlog;

int nb_forces = 0;
float *forces_list;
int *forces_atoms_list;

void MDDriver_init(const char *hostname, int port) {
    int mode = 0;
    int wait = 0;
    int IMDmsg = 0;
    // int IMDmsg = 3;
    int IMDPort = port;
    N_atoms = 0;
    int tries = 20;//20*500ms = 10 seconds
    int t = 0;

    while (!IIMD_probeconnection()) {
        IIMD_init( hostname, &mode, &wait, &IMDPort, &IMDmsg , 0);

#if defined(_WIN32)
        Sleep(500);
#else
        usleep(500000);
#endif
        t++;
        if (t >= tries)
            break;
    }

}

int MDDriver_start()
{
    // int rc;
    // rc = pthread_create(&imdthreads, NULL, run, (void *) this);
    // if (rc)
    //     return 0;
    // return 1;
    return 1;

}

void freeForces() {
    if (forces_list != NULL) {
        free(forces_list);
        free(forces_atoms_list);
        forces_list = NULL;
        forces_atoms_list = NULL;
    }
    nb_forces = 0;
}

int MDDriver_stop() {
    if (IIMD_probeconnection()) {
        IIMD_send_kill();
        IIMD_send_disconnect();
    }
    IIMD_terminate ();
    N_atoms = 0;
    freeForces();
    return 1;
}

void MDDriver_disconnect() {
    if (IIMD_probeconnection()) {
        IIMD_send_disconnect();
    }
    IIMD_terminate();
    N_atoms = 0;

    freeForces();
}


bool MDDriver_isConnected() {
    return IIMD_probeconnection() == 1;
}

int MDDriver_getNbParticles() {
    if (N_atoms <= 0) {
        IIMD_get_coords( &N_atoms, (float **) &coords );
    }
    return N_atoms;
}

void MDDriver_pause() {
    IIMD_send_pause();
}

void MDDriver_play() {
    IIMD_send_pause();
}

int MDDriver_getPositions(float * coordinates, int nbPos) {
    IIMD_get_coords( &N_atoms, (float **) &coords );
    memcpy(coordinates, coords, nbPos * sizeof(float) * 3);
    return nbPos;
}

void MDDriver_setForces(int nbforces, int *atomslist, float * forceslist) {
    if (nb_forces != nbforces) {
        if (forces_list != NULL) {
            free(forces_list);
            free(forces_atoms_list);
        }
        forces_list = (float *)malloc(sizeof(float) * 3 * nbforces);
        forces_atoms_list = (int *)malloc(sizeof(int) * nbforces);
    }
//mettre les precedentes forces a 0 ?
    memcpy(forces_list, forceslist, nbforces * sizeof(float) * 3);
    memcpy(forces_atoms_list, atomslist, nbforces * sizeof(int));

    nb_forces = nbforces;
}
void MDDriver_resetForces() {
    freeForces();
}

void MDDriver_getEnergies(IMDEnergies **energies) {
    IMDEnergies *curE;
    IIMD_get_energies( &curE);
    *energies = curE;
}

void MDDriver_loop() {
    IIMD_treatprotocol();
    if (nb_forces != 0) {
        IIMD_send_forces( &nb_forces,  forces_atoms_list,  (const float *) forces_list );
    }
    else {
        int *atomlist;
        float *forceList;
        int nf = 0;

        IIMD_get_forces(&nf, &atomlist, &forceList);
        fprintf(MYIMDlog, "Hahah ! %d", nf);
        if (nf > 0) {
            fprintf(MYIMDlog, "%d", atomlist[0]);
        }

    }
}
