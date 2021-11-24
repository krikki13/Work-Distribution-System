# CMakeList.txt : CMake project for Worker, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
#add_executable (Worker "Worker.cpp" "Worker.h")

# TODO: Add tests and install targets if needed.
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.74.0 COMPONENTS system regex date_time) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable (Master "Master.cpp" "Master.h")
    add_executable (server "server.cpp")
    add_executable (client "client.cpp")
    target_link_libraries(Master ${Boost_LIBRARIES})
    target_link_libraries(server ${Boost_LIBRARIES})
    target_link_libraries(client ${Boost_LIBRARIES})
endif()
