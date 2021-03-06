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

INCLUDE_DIRECTORIES(include)
ADD_LIBRARY(mddriver src/imd.c  src/imd_interface.c  src/vmdsock.c)
ADD_LIBRARY(mddrivershared SHARED src/imd.c  src/imd_interface.c  src/vmdsock.c)
#MDDRIVER LIBRARY
ADD_EXECUTABLE (protocoltest src/protocoltest.c)
TARGET_LINK_LIBRARIES(protocoltest mddriver)
ADD_EXECUTABLE (servertest src/servertest.c)
TARGET_LINK_LIBRARIES(servertest mddriver)
ADD_EXECUTABLE (clienttest src/clienttest.c)
TARGET_LINK_LIBRARIES(clienttest mddriver)
INSTALL(TARGETS clienttest servertest protocoltest RUNTIME DESTINATION ${MODULE_DIR})

INSTALL(TARGETS mddriver ARCHIVE DESTINATION ${LIB_DIR})
INSTALL(TARGETS mddrivershared LIBRARY DESTINATION ${LIB_DIR})

IF (UNITYMOL_PLUGIN)
  IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    INSTALL(FILES ${LIB_DIR}/libmddrivershared.dylib RENAME libmddrivershared.bundle DESTINATION ${PLUGINS_DIR})
  ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
ENDIF(UNITYMOL_PLUGIN)

INSTALL(DIRECTORY doc DESTINATION ${DOC_DIR} PATTERN .svn EXCLUDE)
INSTALL(FILES include/imd.h include/imd_interface.h include/vmdsock.h DESTINATION ${INCLUDE_DIR})
INSTALL(FILES Doxyfile DESTINATION ${DOC_DIR})
