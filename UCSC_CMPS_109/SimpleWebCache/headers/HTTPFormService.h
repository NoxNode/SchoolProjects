#ifndef HTTPFORMSERVICE_H
#define HTTPFORMSERVICE_H
#include "HTTPService.h"
// An HTTP Service that serves HTML Form posts and parses 
class HTTPFORMService: public HTTPService // Inherit from HTTPService Base class
{
    protected:
        std::map <string,string> form_data;
        std::map <string,string> raw_form_data;
        string urldecode(string & urlencoded);
        void add_to_form_data(string & form_field);
        void parse_data (HTTPRequest * p_httpRequest);
        string compose_reply();
    public:
        // Constructor, receiving file cache, resources to serve and a flag that enabled deleting the cache in the destructor
        HTTPFORMService();
        // Execute the service and write back the results to the TCPSocket
        virtual bool execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket);
        virtual HTTPService * clone (); // Clone and create a new object
        ~HTTPFORMService(); // Destructor
};

#endif // HTTPFORMSERVICE_H
