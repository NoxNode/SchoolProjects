#ifndef HTTPNOTACCEPTABLEEXCEPTIONHANDLER_H
#define HTTPNOTACCEPTABLEEXCEPTIONHANDLER_H

#include "HTTPExceptionHandler.h"
// Exception Handler for HTTP Request Not Acceptable. Basically sending back to the client status code 406
class HTTPNotAcceptableExceptionHandler: public HTTPExceptionHandler
{ // Inheriting from the HTTPExceptionHandler Base class
    private:
    public:
        HTTPNotAcceptableExceptionHandler(); // Constructor
        //The Exception handler method
        virtual void handle (TCPSocket * p_tcpSocket) ;
        ~HTTPNotAcceptableExceptionHandler(); // Destructor
};

#endif // HTTPNOTACCEPTABLEEXCEPTIONHANDLER_H