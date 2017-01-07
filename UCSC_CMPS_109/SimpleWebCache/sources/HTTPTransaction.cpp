#include "HTTPTransaction.h"
#include "HTTPGETRequest.h"
#include "HTTPNotAcceptableExceptionHandler.h"
#include "HTTPMethodNotAllowedExceptionHandler.h"
// Constructor: initailizing data members and calling the Thread Base class constructor
HTTPTransaction::HTTPTransaction (TCPSocket * p_tcpSocket,HTTPServiceManager * p_httpServiceManager,HTTPRequestManager * p_httpRequestManager):
    Thread()
{ 
    httpServiceManager = p_httpServiceManager;
    tcpSocket = p_tcpSocket;
    httpRequestManager = p_httpRequestManager;
};
// Instantiate the HTTPRequest object based on the method type of the request
HTTPRequest * HTTPTransaction::fetchHTTPRequest ()
{
    // buffer to read very small amount of data from socket. Enough to get the method name
    char buffer[100];
    memset (buffer,0,100);
    tcpSocket->readFromSocket(buffer,99); // read from socket
    stringstream iss(buffer); // stringstream for parsing buffer
    string method="";
    getline(iss,method,' '); // extract whaterver before a white space into method
    // Invoke the httpRequestFactory for an httpRequest object of type equivelant to the request method
    HTTPRequest * httpRequest = httpRequestManager->getService(tcpSocket,method);    
    // if an object is returned then execute the readAndParse method else an exception will be thrown
    if (httpRequest != NULL ) httpRequest->readAndParse(buffer);
    return httpRequest; // return the httpRequest object
}
// Process the HTTP transaction: main function
void HTTPTransaction::process()
{
    HTTPRequest * httpRequest; // Define a pointer to an HTTPRequest object
    try{ // try the following code block and look for exceptions
        // Fetch the httpRequest object needed to service this request based on the method type
        httpRequest= fetchHTTPRequest (); 
        // Invoke the HTTPServiceManager for a service that can serve the request resource
        HTTPService * s  =httpServiceManager->getService(httpRequest->getResource());
        s->execute(httpRequest,tcpSocket); // Execute the servive
        delete (httpRequest); // delete the httpRequest object
        tcpSocket->shutDown(); // Shutdown the TCP socket
    }
    catch (HTTPNotAcceptableExceptionHandler httpNotAcceptableExceptionHandler )
    { //An exception occurred indicating that the service requested is not accepted 
        httpNotAcceptableExceptionHandler.handle(tcpSocket); // handle exception
        delete (httpRequest); // delete the httpRequest object
        tcpSocket->shutDown(); // Shutdown the TCP socket
    }
    catch (HTTPMethodNotAllowedExceptionHandler httpMethodNotAllowedExceptionHandler )
    { //An exception occurred indicating that the requested resource cannot be found or accessed
        httpMethodNotAllowedExceptionHandler.handle(tcpSocket);// handle exception
        tcpSocket->shutDown(); // Shutdown the TCP socket
    }
    
}
// Main thread body entry point
void * HTTPTransaction::threadMainBody (void * arg)
{
    // Cast the arg to (HTTPTransaction *)
    HTTPTransaction * thisHTTPTransaction = (HTTPTransaction *) arg;
    thisHTTPTransaction->process(); // invoke process
    return NULL; // return NULL anyways
}
// Destructor
HTTPTransaction::~HTTPTransaction()
{
    delete (tcpSocket); // Delete the tcpSocket object
}
