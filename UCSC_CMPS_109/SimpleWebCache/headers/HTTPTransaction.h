#ifndef HTTPTRANSACTION_H
#define HTTPTRANSACTION_H
#include "Thread.h"
#include "HTTPServiceManager.h"
#include "HTTPRequest.h"
#include "HTTPRequestManager.h"

// A class encapsulating all the details of an HTTP Transaction. It inherits from Thread as every HTTP transaction should be executed as a detached paralled independant thread
class HTTPTransaction : public Thread 
{
    private:
        TCPSocket * tcpSocket; // TCP socket of the target HTTP transaction
        HTTPServiceManager * httpServiceManager; // Pointer to the HTTPServiceManager to be able to fetch services
        HTTPRequestManager * httpRequestManager; // Pointer to the HTTPRequestManager to be able to fetch a request handler based on the request method type
        HTTPRequest * fetchHTTPRequest (); // Return an HTTPRequest object whose type depends on the HTTPHeader method
        void process(); // Process the HTTP transaction
    public:
        // Constructor setting up the data members of the HTTPTRansaction object
        HTTPTransaction (TCPSocket * p_tcpSocket,HTTPServiceManager * p_httpServiceManager,HTTPRequestManager * p_httpRequestManager);
        void * threadMainBody (void * arg); // The main thread body that executed all the logic for the detached thread
        ~HTTPTransaction(); // Destructor
};

#endif //HTTPTRANSACTION_H
