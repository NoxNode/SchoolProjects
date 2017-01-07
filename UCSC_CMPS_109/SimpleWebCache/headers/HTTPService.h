#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H
#include "FileCache.h"
#include "TCPSocket.h"
#include "HTTPRequest.h"

class HTTPService // An abstract class representing an HTTP Service
{
    protected:
        FileCache * fileCache; // A file cache
        bool clean_cache; // A flag indicating whether the destructor should destruct the fileCache or not
        long getTimeStamp (string date_string);
    public:
        // Constructor setting up the objec data members
        HTTPService(FileCache * p_fileCache,bool p_clean_cache = true);
        // A pure virtual method of executing the HTTP back end which should generate the HTTP response body and send it to the client over the current connection TCP socket
        virtual bool execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket) = 0;
        // A pure virtual method that should be implemented by all descendants to clone and create new object        
        virtual HTTPService * clone () = 0;
        virtual ~HTTPService(); // Destructor
};

#endif // HTTPSERVICE_H
