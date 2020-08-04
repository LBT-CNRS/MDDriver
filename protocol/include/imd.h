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
	IMD_IOERROR       //!< indicate an I/O error
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

// Swap little <-> big endian
extern void imd_swap4(char *data, int ndata);

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

//! Send energies
extern int imd_send_energies(void *, const IMDEnergies *);

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
 *  Receive energies
 */
extern int imd_recv_energies(void *, IMDEnergies *);

/*!
 *  Receive atom coordinates and forces
 *  Forces are in Kcal/mol/angstrom
 */
extern int imd_recv_fcoords(void *, imd_int32, float *);

#endif

