cmake_minimum_required(VERSION 3.15)

project(MDDriver VERSION 1.0)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)


option(BUILD_PLUGINS "Build Plugins" ON)
option(INSTALL_EXAMPLE_TOOL "Install the example tool which use the MDDriver library" OFF)



#### Installation directories ####

# Set the installation location of mddriver components.
set(BIN_INSTALL_DIR         bin)
set(LIB_INSTALL_DIR         lib)
set(INCLUDE_INSTALL_DIR     include)

set(MDDRIVER_INSTALL_SHARE_DIR share)

set(DOC_INSTALL_DIR         ${MDDRIVER_INSTALL_SHARE_DIR}/doc)
set(PLUGINS_INSTALL_DIR     ${MDDRIVER_INSTALL_SHARE_DIR}/plugins)
set(CMAKEMOD_INSTALL_DIR    ${MDDRIVER_INSTALL_SHARE_DIR}/cmake)



#################
### PROTOCOL  ###
#################

add_subdirectory(protocol)

#################
### PLUGINS  ####
#################

if(BUILD_PLUGINS)
    add_subdirectory(plugins)
endif()

#####################
### EXAMPLE TOOL ####
#####################

if(INSTALL_EXAMPLE_TOOL)
    install(DIRECTORY example_tool
            DESTINATION ${MDDRIVER_INSTALL_SHARE_DIR}
            )
endif()

###################
# Configure files #
###################

configure_file(${PROJECT_SOURCE_DIR}/config/MDDriver-config.sh.in
    ${PROJECT_BINARY_DIR}/MDDriver-config.sh @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/config/MDDriver-config.csh.in
    ${PROJECT_BINARY_DIR}/MDDriver-config.csh @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/config/MDDriverConfig.cmake.in
    ${PROJECT_BINARY_DIR}/MDDriverConfig.cmake @ONLY)


# install configure files
install(FILES
    ${PROJECT_BINARY_DIR}/MDDriver-config.sh
    ${PROJECT_BINARY_DIR}/MDDriver-config.csh
    DESTINATION ${BIN_INSTALL_DIR}
)

# install cmake file
install(FILES
    ${PROJECT_BINARY_DIR}/MDDriverConfig.cmake
    DESTINATION ${CMAKEMOD_INSTALL_DIR}
)


MESSAGE(STATUS "Please source ${CMAKEMOD_INSTALL_DIR}/MDDriver-config.sh or ${CMAKEMOD_INSTALL_DIR}/MDDriver-config.sh including environment variables for MDDRiver after compiling and installing")
