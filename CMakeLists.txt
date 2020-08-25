# Copyright Centre National de la Recherche Scientifique (CNRS)
# contributors :
# Marc Baaden, 2007-8
# Olivier Delalande, 2008
# Nicolas Ferey, 2008
# Gilles Grasseau, 2007-8
#
# baaden@smplinux.de
# http://www.baaden.ibpc.fr
#
# This software is a computer program whose purpose is to provide a general
# interface for coupling calculation and visualization modules via the IMD
# protocol. The part of this source code dealing directly with the IMD
# protocol was adapted from the VMD/NAMD packages.
#
# This source code is part of MDDriver, a general purpose library for
# interactive simulations. This software is governed by the CeCILL-C license
# under French law and abiding by the rules of distribution of free software.
# You can use, modify and/or redistribute the software under the terms of the
# CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL
# http://www.cecill.info.
#
# As a counterpart to the access to the source code and rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights, and the successive licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading, using, modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean that it is complicated to manipulate, and that also
# therefore means that it is reserved for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and, more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL-C license and that you accept its terms.

PROJECT (MDDriver)

ADD_DEFINITIONS(-fPIC)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
      CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(CMAKE_C_FLAGS "-D__MACOSX__")
	SET(CMAKE_C_FLAGS "-arch x86_64  -D__MACOSX__")
	#SET(CMAKE_C_FLAGS "-arch i386 -arch ppc -arch x86_64 -mmacosx-version-min=10.6  -D__MACOSX__")
	#SET(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.6.sdk)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET(CMAKE_C_FLAGS "-D__LINUX__")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(WIN32)
    link_libraries(ws2_32 wsock32)
ENDIF()


# by default, we'll be installing mddriver in /some/standard/path/MDDriver
#IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#    SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}
#        CACHE
#        PATH
#        "Install path prefix, prepended onto install directories."
#        FORCE
#        )
#ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)


SET(INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/include)
SET(LIB_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/lib)
SET(MODULE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/bin)
SET(DATA_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/data)
SET(CMAKE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/cmake)
SET(DOC_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}/doc)


CONFIGURE_FILE(config/${CMAKE_PROJECT_NAME}-config.sh.in ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-config.sh @ONLY)
CONFIGURE_FILE(config/${CMAKE_PROJECT_NAME}-config.csh.in ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-config.csh @ONLY)
CONFIGURE_FILE(config/${CMAKE_PROJECT_NAME}Config.cmake.in ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake @ONLY)

# install configured files
INSTALL(FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-config.sh ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-config.csh
    DESTINATION ${MODULE_DIR}
    PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
    OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

INSTALL(FILES ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    DESTINATION ${CMAKE_DIR})

SUBDIRS(protocol adapters)
INSTALL(DIRECTORY data
    DESTINATION ${DATA_DIR}
    PATTERN .svn EXCLUDE)
INSTALL(DIRECTORY adapters
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}
    PATTERN .svn EXCLUDE)
INSTALL(FILES INSTALL README.md LICENSE
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME})


MESSAGE(STATUS "Please source ${MODULE_DIR}/MDDriver-config.sh or ${MODULE_DIR}/MDDriver-config.sh including environment variables for MDDRiver after compiling and installing")
