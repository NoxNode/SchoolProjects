#pragma once
/*
* given in class
* modifications labeled with "we added this"
*/

#include "Thread.h"
#include "TCPSocket.h"
#include "MISProgram.h" // we add this - includes MISProgram

// A class representing a single connection form a client
 // we removed the next connection because we put that functionality in the ThreadManager class
class Connection : public Thread { //Inherit from Thread
private:
    MISProgram* program; // we added this - Connection has a pointer to a program
    TCPSocket * tcpSocket; // TCP Socket for communication with client
public:
    Connection(TCPSocket * p_tcpSocket); // Constructor: Set client connected TCP socket
    void * threadMainBody (void * arg); // Main thread body that serves the connection
    TCPSocket* getSock(); // we added this func, gets a pointer to the socket
    virtual ~Connection(); // Destructor
};
