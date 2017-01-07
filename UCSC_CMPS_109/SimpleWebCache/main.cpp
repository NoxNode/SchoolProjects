/* 
 * File:   main.cpp
 * Author: kmsobh
 *
 * Created on October 21, 2016, 12:20 PM
 */

#include "TCPServerSocket.h"
#include "Thread.h"
#include "HTTPTransaction.h"
#include "GarbageCollector.h"
#include "HTTPRequestManager.h"

using namespace std;

int main(int argc, char** argv) {
    // Instantiate a garbage collector object
    GarbageCollector * garbageCollector = new GarbageCollector();
    // Instantiate an HTTPServiceManager Factory
    HTTPServiceManager * httpServiceManager = new HTTPServiceManager();
    // Instantiate an HTTPRequestManager Factory  
    HTTPRequestManager * httpRequestManager = new HTTPRequestManager();
    // Create a TCP socket that listens on the localhost loopback address and port 9999
    TCPServerSocket * tcpServerSocket = new TCPServerSocket("127.0.0.1",9999,100);
    tcpServerSocket->initializeSocket(); // Initialize the server socket
    while (true) // loop until socket errors
    {
        // Wait for connection indefinitely
        TCPSocket * tcpSocket = tcpServerSocket->getConnection(0,0,-1,-1);
        if ( tcpSocket ==NULL) break; // if returns a NULL break the loop and exist
        garbageCollector->cleanup(); // Apply garbage collection cleanup
        // Instantiate a new HTTPTransaction Object and pass the TCPSocket pointer and different factories to it
        HTTPTransaction * httpTransaction = new HTTPTransaction(tcpSocket,httpServiceManager,httpRequestManager);
        // Add the HTTPTransaction just created to the garbage collector for clean up
        garbageCollector->addHTTPTransaction(httpTransaction);
        // Start the HTTPTransaction thread
        httpTransaction->start();
    }
    
    delete (tcpServerSocket); // Delete the TCPServerSocket
    delete (httpServiceManager); // Delete the HTTPServiceManager
    delete (httpRequestManager); // Delete the HTTPRequestManager
    delete (garbageCollector); // Delete the Garbage Collector
    return 0;
}

