#pragma once
#include <iostream>
#include <exception>

using namespace std;

class ServerException : public std::exception {
public:
    ServerException(string msg) {
        message = msg;
    }

    const char* what() const throw () {
        return message.c_str();
    }

private:
    string message;
};