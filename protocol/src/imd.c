/** @file imd.c
 * @brief Source file for IMD routines
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
 *      $RCSfile: imd.c,v $
 *      $Author: mdd $       $Locker:  $             $State: Exp $
 *      $Revision: 1.2 $       $Date: 2008-06-26 13:44:13 $
 *
 * Id: imd.c,v 1.1 2008-03-20 14:52:57 mdd Exp $
 ***************************************************************************/

/***************************************************************************
 *
 * MODIFICATIONS (July 2007)
 * -------------
 *     Replaced 'int32' by 'imd_int32' (conflicts with xlC types)
 *     Proposed by M.Baaden (IBPC/CNRS) and G.Grasseau (IDRIS/CNRS)
 *
 ***************************************************************************/

#include "imd.h"
#include "vmdsock.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>

//! Header for IMD connections
typedef struct 
	{
	imd_int32 type;
	imd_int32 length;
	} IMDheader;

#define HEADERSIZE 8
#define IMDVERSION 2

void imd_swap4(char *data, int ndata) 
	{
	int i;
	char *dataptr;
	char b0, b1;

	dataptr = data;
	for (i=0; i<ndata; i+=4) 
		{
		b0 = dataptr[0];
		b1 = dataptr[1];
		dataptr[0] = dataptr[3];
		dataptr[1] = dataptr[2];
		dataptr[2] = b1;
		dataptr[3] = b0;
		dataptr += 4;
		}
	}

static imd_int32 imd_htonl(imd_int32 h) 
	{
	imd_int32 n;
	((char *)&n)[0] = (h >> 24) & 0x0FF;
	((char *)&n)[1] = (h >> 16) & 0x0FF;
	((char *)&n)[2] = (h >> 8) & 0x0FF;
	((char *)&n)[3] = h & 0x0FF;
	return n;
	}

//! netint structure definition
typedef struct 
	{
	unsigned int highest : 8;
	unsigned int high    : 8;
	unsigned int low     : 8;
	unsigned int lowest  : 8;
	} netint;

static imd_int32 imd_ntohl(imd_int32 n) 
	{
	imd_int32 h = 0;
	netint net;
	net = *((netint *)&n);
	h |= net.highest << 24 | net.high << 16 | net.low << 8 | net.lowest;
	return h;
	}

static void fill_header(IMDheader *header, IMDType type, imd_int32 length) 
	{
	header->type = imd_htonl((imd_int32)type);
	header->length = imd_htonl(length);
	}

static void swap_header(IMDheader *header) 
	{
	header->type = imd_ntohl(header->type);
	header->length= imd_ntohl(header->length);
	}

static imd_int32 imd_readn(void *s, char *ptr, imd_int32 n) 
	{
	imd_int32 nleft;
	imd_int32 nread;

	nleft = n;
	while (nleft > 0) 
		{
		if ((nread = vmdsock_read(s, ptr, nleft)) < 0) 
			{
			if (errno == EINTR)
				nread = 0;         /* and call read() again */
			else
				return -1;
			} 
		else if (nread == 0)
			break;               /* EOF */
		nleft -= nread;
		ptr += nread;
		}
	return n-nleft;
	}

static imd_int32 imd_writen(void *s, const char *ptr, imd_int32 n) 
	{
	imd_int32 nleft;
	imd_int32 nwritten;

	nleft = n;
	while (nleft > 0) 
	{
		if ((nwritten = vmdsock_write(s, ptr, nleft)) <= 0) 
			{
			if (errno == EINTR)
				nwritten = 0;
			else
				return -1;
			}
		nleft -= nwritten;
		ptr += nwritten;
		}
	return n;
	}


int imd_disconnect(void *s) 
	{
	IMDheader header;
	fill_header(&header, IMD_DISCONNECT, 0);
	return (imd_writen(s, (char *)&header, HEADERSIZE) != HEADERSIZE);
	}

int imd_pause(void *s) 
	{
	IMDheader header;
	fill_header(&header, IMD_PAUSE, 0);
	return (imd_writen(s, (char *)&header, HEADERSIZE) != HEADERSIZE);
	}

int imd_kill(void *s) 
	{
	IMDheader header;
	fill_header(&header, IMD_KILL, 0);
	return (imd_writen(s, (char *)&header, HEADERSIZE) != HEADERSIZE);
	}

static int imd_go(void *s) 
	{
	IMDheader header;
	fill_header(&header, IMD_GO, 0);
	return (imd_writen(s, (char *)&header, HEADERSIZE) != HEADERSIZE);
	}


int imd_handshake(void *s) 
	{
	IMDheader header;
	fill_header(&header, IMD_HANDSHAKE, 1);
	header.length = IMDVERSION;   /* Not byteswapped! */
	return (imd_writen(s, (char *)&header, HEADERSIZE) != HEADERSIZE);
	}

int imd_trate(void *s, imd_int32 rate) 
	{
	IMDheader header;
	fill_header(&header, IMD_TRATE, rate);
	return (imd_writen(s, (char *)&header, HEADERSIZE) != HEADERSIZE);
	}

/* Data methods */
int imd_send_mdcomm(void *s,imd_int32 n,const imd_int32 *indices,const float *forces) 
	{
	int rc;
	imd_int32 size = HEADERSIZE+16*n;
	char *buf = (char *) malloc(sizeof(char) * size); 
	fill_header((IMDheader *)buf, IMD_MDCOMM, n);
	memcpy(buf+HEADERSIZE, indices, 4*n);
	memcpy(buf+HEADERSIZE+4*n, forces, 12*n);
	rc = (imd_writen(s, buf, size) != size);
	free(buf);
	return rc;
	}

int imd_send_energies(void *s, const IMDEnergies *energies) 
	{
	int rc;
	imd_int32 size = HEADERSIZE+sizeof(IMDEnergies);
	char *buf = (char *) malloc(sizeof(char) * size);
	fill_header((IMDheader *)buf, IMD_ENERGIES, 1);
	memcpy(buf+HEADERSIZE, energies, sizeof(IMDEnergies));
	rc = (imd_writen(s, buf, size) != size);
	free(buf);
	return rc;
	}

int imd_send_fcoords(void *s, imd_int32 n, const float *coords) 
	{
	int rc;
	imd_int32 size = HEADERSIZE+12*n;
	char *buf = (char *) malloc(sizeof(char) * size); 
	fill_header((IMDheader *)buf, IMD_FCOORDS, n);
	memcpy(buf+HEADERSIZE, coords, 12*n);
	rc = (imd_writen(s, buf, size) != size);
	free(buf);
	return rc;
	}

/* The IMD receive functions */
IMDType imd_recv_header_nolengthswap(void *s, imd_int32 *length) 
	{
	IMDheader header;
	if (imd_readn(s, (char *)&header, HEADERSIZE) != HEADERSIZE)
	return IMD_IOERROR;
	*length = header.length;
	swap_header(&header);
	return (IMDType) header.type; 
	}

IMDType imd_recv_header(void *s, imd_int32 *length) 
	{
	IMDheader header;
	if (imd_readn(s, (char *)&header, HEADERSIZE) != HEADERSIZE)
	return IMD_IOERROR;
	swap_header(&header);
	*length = header.length;
	return (IMDType) header.type; 
	}

int imd_recv_handshake(void *s) 
	{
	imd_int32 buf;
	IMDType type;

	/* Wait up to 5 seconds for the handshake to come */
	if (vmdsock_selread(s, 5) != 1) 
		return -1;

	/* Check to see that a valid handshake was received */
	type = imd_recv_header_nolengthswap(s, &buf);
	if (type != IMD_HANDSHAKE) 
		return -1;

	/* Check its endianness, as well as the IMD version. */
	if (buf == IMDVERSION) 
		{
		if (!imd_go(s)) 
			return 0;
		return -1;
		}
	imd_swap4((char *)&buf, 4);
	if (buf == IMDVERSION) 
		{
		if (!imd_go(s)) 
			return 1;
		}

	/* We failed to determine endianness. */
	return -1; 
	}

int imd_recv_mdcomm(void *s, imd_int32 n, imd_int32 *indices, float *forces) 
	{
	if (imd_readn(s, (char *)indices, 4*n) != 4*n) return 1;
		if (imd_readn(s, (char *)forces, 12*n) != 12*n) return 1;
			return 0;
	}

int imd_recv_energies(void *s, IMDEnergies *energies) 
	{
	return (imd_readn(s, (char *)energies, sizeof(IMDEnergies))!= sizeof(IMDEnergies));
	}

int imd_recv_fcoords(void *s, imd_int32 n, float *coords) 
	{
	return (imd_readn(s, (char *)coords, 12*n) != 12*n);
	}

