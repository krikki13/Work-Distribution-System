# CMakeList.txt : CMake project for WorkerController, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# TODO: Add tests and install targets if needed.
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.74.0 COMPONENTS system regex date_time) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable (WorkerController "WorkerController.cpp" "WorkerController.h")
    target_link_libraries(WorkerController ${Boost_LIBRARIES})
endif()
