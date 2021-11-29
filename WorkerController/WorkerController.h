// Worker.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp> 
#include <boost/lexical_cast.hpp>
#include <boost/predef.h>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <thread>
#include <mutex>
#include <memory>
#include <list>

#include "../common/TcpServer/TcpClient.cpp"
#include "../common/WorkerState.cpp"
