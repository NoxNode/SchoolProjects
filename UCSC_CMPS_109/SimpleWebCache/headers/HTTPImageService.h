#ifndef HTTPIMAGESERVICE_H
#define HTTPIMAGESERVICE_H
#include "HTTPService.h"

// edit by Mark Palladino
	// copy pasted this class from HTTPHTMLService and just renamed everyting and added the "string ext"
// An HTTP Service that serves Image files
class HTTPImageService: public HTTPService // Inherit from HTTPService Base class
{
    private:
		string ext; // the extension of the image (png, jpg, or gif)
    public:
        // Constructor, receiving file cache, resources to serve and a flag that enabled deleting the cache in the destructor
		// passing in the extension in the first parameter to not mess with default values
        HTTPImageService(string _ext, FileCache * p_fileCache,bool p_clean_cache = true);
        // Execute the service and write back the results to the TCPSocket
        virtual bool execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket);
        virtual HTTPService * clone (); // Clone and create a new object
        ~HTTPImageService(); // Destructor
};

#endif /* HTTPIMAGESERVICE_H */
