#ifndef HTTPHTMLSERVICE_H
#define HTTPHTMLSERVICE_H
#include "HTTPService.h"
// An HTTP Service that serves HTML files
class HTTPHTMLService: public HTTPService // Inherit from HTTPService Base class
{
    private:
    public:
        // Constructor, receiving file cache, resources to serve and a flag that enabled deleting the cache in the destructor
        HTTPHTMLService(FileCache * p_fileCache,bool p_clean_cache = true);
        // Execute the service and write back the results to the TCPSocket
        virtual bool execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket);
        virtual HTTPService * clone (); // Clone and create a new object
        ~HTTPHTMLService(); // Destructor
};

#endif // HTTPHTMLSERVICE_H
