
cmake_minimum_required(VERSION 3.15)

project(MDDriver_tool)

#MDDRIVER LIBRARY
find_package(MDDriver REQUIRED)

if(MDDriver_FOUND)
	message(STATUS "MDDriver found: ")
	message(STATUS ${MDDRIVER_INCLUDE_DIR})
	message(STATUS ${MDDRIVER_LIBRARY})
endif()

find_package(Threads REQUIRED)

add_executable(MDDriver_tool src/main.cpp src/imdLoop.cpp src/MDDriver.cpp src/mddriverforce.cpp)
target_include_directories(MDDriver_tool PRIVATE ${MDDRIVER_INCLUDE_DIR} include/)
target_link_libraries(MDDriver_tool PRIVATE ${MDDRIVER_LIBRARY})

