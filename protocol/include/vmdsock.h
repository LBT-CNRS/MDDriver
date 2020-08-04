/** @file vmdsock.h
 * @brief Header file for socket management
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
 *      $RCSfile: vmdsock.h,v $
 *      $Author: mdd $        $Locker:  $             $State: Exp $
 *      $Revision: 1.2 $      $Date: 2008-06-25 11:56:07 $
 *
 ***************************************************************************
 * DESCRIPTION:
 *   socket interface layer, abstracts platform-dependent routines/APIs
 ***************************************************************************/

#if defined(VMDSOCKINTERNAL)

	#if !defined(_MSC_VER)
		#include <unistd.h>
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <sys/time.h>
		#include <netinet/in.h>
		#include <sys/file.h>
	#endif

	//! Bug? Why does this description not show up in doxygen ??
	typedef struct 
		{
		  struct sockaddr_in addr; //!< address of socket provided by bind()
		  int addrlen;             //!< size of the addr struct
		  int sd;                  //!< socket file descriptor
		} vmdsocket;

#endif /* VMDSOCKINTERNAL */

#ifdef __cplusplus
	extern "C" 
		{
#endif
		  int   vmdsock_init(void);
		  void *vmdsock_create(void);
		  int   vmdsock_bind(void *, int);
		  int vmdsock_host_bind(void *, int , int);
		  int   vmdsock_listen(void *);
		  void *vmdsock_accept(void *);  //!< return new socket */
		  int   vmdsock_connect(void *, const char *, int);
		  int   vmdsock_write(void *, const void *, int);
		  int   vmdsock_read(void *, void *, int);
		  int   vmdsock_selread(void *, int);
		  int   vmdsock_selwrite(void *, int);
		  void  vmdsock_shutdown(void *);
		  void  vmdsock_destroy(void *);
#ifdef __cplusplus
		}
#endif

