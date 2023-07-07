######################
### UnityMol PLugin #
######################
add_library(Unity_MDDriver SHARED UnityMol/UMolMDDriver.cpp)
target_link_libraries(Unity_MDDriver PUBLIC mddriver)


INSTALL(TARGETS Unity_MDDriver LIBRARY DESTINATION ${PLUGINS_INSTALL_DIR}/UnityMol)
