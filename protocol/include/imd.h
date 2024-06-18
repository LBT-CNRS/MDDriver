/*! @file imd.h
 * @brief Header file for IMD routines
 * @author This source code was adapted from the VMD/NAMD packages.
 *
 */

/***************************************************************************
 *cr
 *cr            (C) Copyright 1995-2003 The Board of Trustees of the
 *cr                        University of Illinois
 *cr                         All Rights Reserved
 *cr
 ***************************************************************************/

/***************************************************************************
 * RCS INFORMATION:
 *
 *      $RCSfile: imd.h,v $
 *      $Author: mdd $       $Locker:  $             $State: Exp $
 *      $Revision: 1.2 $       $Date: 2008-06-25 11:56:07 $
 *
 ***************************************************************************/

/***************************************************************************
 *
 * MODIFICATIONS (June 2009)
 * -------------
 *     Replaced 'int32' by 'imd_int32' (conflicts with xlC types)
 *     Proposed by M.Baaden (IBPC/CNRS) and G.Grasseau (IDRIS/CNRS)
 *
 * $Id: imd.h,v 1.2 2008-06-25 11:56:07 mdd Exp $
 ***************************************************************************/

#ifndef IMD_H__
#define IMD_H__

#include <limits.h>
// ??? Voir avec Marc
// #include <cfortran.h>


#if ( INT_MAX == 2147483647 )
typedef int     imd_int32;
#else
typedef short   imd_int32;
#endif
typedef long long  imd_int64;

#define DATANAME_SIZE 8
#define HEADERSIZE 8
#define IMDVERSION 2


typedef enum IMDType_t
{
	IMD_DISCONNECT,   //!< close IMD connection, leaving sim running
	IMD_ENERGIES,     //!< energy data block
	IMD_FCOORDS,      //!< atom coordinates
	IMD_GO,           //!< start the simulation
	IMD_HANDSHAKE,    //!< endianism and version check message
	IMD_KILL,         //!< kill the simulation job, shutdown IMD
	IMD_MDCOMM,       //!< MDComm style force data
	IMD_PAUSE,        //!< pause the running simulation
	IMD_TRATE,        //!< set IMD update transmission rate
	IMD_IOERROR,      //!< indicate an I/O error
	IMD_GRID,       	//!< grid data block
	IMD_CUSTOM_FLOAT, //!< float data block
	IMD_CUSTOM_INT  //!< int data block
} IMDType;


/*! \brief Energy exchange
 *
 * The timestep, temperature and energies that are defined here are
 * visualized in VMD when activating an IMD connection.
 */


typedef struct
{
	imd_int32 tstep;  //!< integer timestep index
	float T;          //!< Temperature in degrees Kelvin
	float Etot;       //!< Total energy, in Kcal/mol
	float Epot;       //!< Potential energy, in Kcal/mol
	float Evdw;       //!< Van der Waals energy, in Kcal/mol
	float Eelec;      //!< Electrostatic energy, in Kcal/mol
	float Ebond;      //!< Bond energy, Kcal/mol
	float Eangle;     //!< Angle energy, Kcal/mol
	float Edihe;      //!< Dihedral energy, Kcal/mol
	float Eimpr;      //!< Improper energy, Kcal/mol
} IMDEnergies;

typedef struct
{
	imd_int32 tstep;  //!< integer timestep index

	float Xorigin;   //!< Grid origin X (diagonal origin)
	float Yorigin;   //!< Grid origin Y (diagonal origin)
	float Zorigin;   //!< Grid origin Z (diagonal origin)

	float Xend;   //!< Grid end X (diagonal origin)
	float Yend;   //!< Grid end Y (diagonal origin)
	float Zend;   //!< Grid end Z (diagonal origin)

	float XdirectionX;   //!< X component X vector direction of the grid frame (right hand frame)
	float YdirectionX;   //!< Y component X vector direction of the grid frame (right hand frame)
	float ZdirectionX;   //!< Z component X vector direction of the grid frame (right hand frame)

	float XdirectionY;   //!< X component Y vector direction of the grid frame (right hand frame)
	float YdirectionY;   //!< Y component Y vector direction of the grid frame (right hand frame)
	float ZdirectionY;   //!< Z component Y vector direction of the grid frame (right hand frame)

	float XdirectionZ;   //!< X component Z vector direction of the grid frame (right hand frame)
	float YdirectionZ;   //!< Y component Z vector direction of the grid frame (right hand frame)
	float ZdirectionZ;   //!< Z component Z vector direction of the grid frame (right hand frame)



	imd_int32 nbcellx;         //!< Number of cells along X axis
	imd_int32 nbcelly;         //!< Number of cells along Y axis
	imd_int32 nbcellz;         //!< Number of cells along Z axis

	float sizecellx;     //!< Size of the cell on x axis
	float sizecelly;     //!< Size of the cell on y axis
	float sizecellz;     //!< Size of the cell on z axis
} IMDGrid;

typedef struct
{
    char *dataname ;
    int nb;
    void *data;
} IMDCustomData;


// Swap little <-> big endian
//extern void imd_swap4(char *data, int ndata);
extern void imd_swap4(void *data, long ndata);

/* Send control messages - these consist of a header with no subsequent data */
extern int imd_disconnect(void *);   //!< leave sim running but close IMD
extern int imd_pause(void *);        //!< pause simulation
extern int imd_kill(void *);         //!< kill simulation, shutdown IMD
extern int imd_handshake(void *);    //!< check endianness, version compat
extern int imd_trate(void *, imd_int32); //!< set IMD update transmission rate

/*! \brief Send data update messages
 *
 *  Send MDComm compatible forces
 *  Forces are in Kcal/mol/angstrom
 */
extern int imd_send_mdcomm(void *, imd_int32, const imd_int32 *, const float *);

/*!
 *  Send custom float array
 */
extern int imd_send_custom_float(void *, const char*, imd_int32, const float *);

/*!
 *  Send custom float array
 */
extern int imd_send_custom_int(void *, const char*, imd_int32, const imd_int32 *);


//! Send energies
extern int imd_send_energies(void *, const IMDEnergies *);

//! Send grid
extern int imd_send_grid(void *, const IMDGrid *);


/*!
 *  Send atom forces and coordinates
 *  Forces are in Kcal/mol/angstrom
 */
extern int imd_send_fcoords(void *, imd_int32, const float *);





/*!
 *  recv_handshake returns 0 if server and client have the same relative
 *  endianism; returns 1 if they have opposite endianism, and -1 if there
 *  was an error in the handshake process.
 */
extern int imd_recv_handshake(void *);

/*!
 *  Receive header and data
 */
extern IMDType imd_recv_header(void *, imd_int32 *);

/*!
 *  Receive MDComm-style forces
 *  Forces are in Kcal/mol/angstrom
 */
extern int imd_recv_mdcomm(void *, imd_int32, imd_int32 *, float *);

/*!
 *  Receive custom float array
 */
extern int imd_recv_custom_float(void * ,  char *, imd_int32, float *);

/*!
 *  Receive custom int array
 */
extern int imd_recv_custom_int(void * ,  char *, imd_int32, imd_int32 *);



/*!
 *  Receive energies
 */
extern int imd_recv_energies(void *, IMDEnergies *);

/*!
 *  Receive grid
 */
extern int imd_recv_grid(void *, IMDGrid *);


/*!
 *  Receive atom coordinates and forces
 *  Forces are in Kcal/mol/angstrom
 */
extern int imd_recv_fcoords(void *, imd_int32, float *);

#endif
