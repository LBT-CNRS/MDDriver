
# MDDriver library
ADD_LIBRARY(mddriver
    src/imd.c
    src/imd_interface.c
    src/vmdsock.c)

target_include_directories(mddriver PUBLIC include/)
# fPIC value
set_property(TARGET mddriver PROPERTY POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    target_link_libraries(mddriver PRIVATE ws2_32 wsock32)
endif()


#MDDRIVER test executable
ADD_EXECUTABLE (protocoltest test_exe/protocoltest.c)
TARGET_LINK_LIBRARIES(protocoltest PRIVATE mddriver)

ADD_EXECUTABLE (servertest test_exe/servertest.c)
TARGET_LINK_LIBRARIES(servertest PRIVATE mddriver)

ADD_EXECUTABLE (clienttest test_exe/clienttest.c)
TARGET_LINK_LIBRARIES(clienttest PRIVATE mddriver)


# install targets
set(TARGETS mddriver clienttest servertest protocoltest)
install(TARGETS ${TARGETS}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

# install headers
install(FILES include/imd.h include/imd_interface.h include/vmdsock.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)

# install doc
INSTALL(FILES Doxyfile DESTINATION ${DOC_INSTALL_DIR})
INSTALL(DIRECTORY doc DESTINATION ${DOC_INSTALL_DIR} USE_SOURCE_PERMISSIONS)
