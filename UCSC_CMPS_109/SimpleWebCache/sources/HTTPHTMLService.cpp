#include "HTTPHTMLService.h"
#include "HTTPResponseHeader.h"
#include "HTTPNotFoundExceptionHandler.h"

HTTPHTMLService::HTTPHTMLService(FileCache * p_fileCache,bool p_clean_cache )
        :HTTPService(p_fileCache,p_clean_cache) {} // Constructor setting data members using initialization list
bool HTTPHTMLService::execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket) {
    try { // Try the following block and look for exceptions
        string resource = p_httpRequest->getResource(); // Fetching the resource from the HTTPRequest object
        FileCacheItem * fileCacheItem = fileCache->getFile(resource); // fetching the resource cache item
        fileCacheItem = fileCacheItem->fetchContent(); // update cache item if needed and return a clone
        // Instantiate an HTTPresponse object and set up its header attributes

		// start edit by Mark Palladino
        string s = p_httpRequest->getHeaderValue("If-Modified-Since"); // get client's modified date from the request header
        char* time_string = fileCacheItem->getLastUpdateTime(); // get modified date from the fileCacheItem
		if(strcmp(s.c_str(), time_string) == 0) { // if they're equal
			// make a new HTTPResponseHeader with the 304 "Not Modified" status code
        	HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"Not Modified",304,"HTTP/1.1");
        	httpResponseHeader->respond(); // Write back the response to the client through the TCPSocket
			delete (httpResponseHeader); // Delete the HTTP Response
			delete (fileCacheItem); // delete the cache item clone
			return true; // return true
		}
		// end edit by Mark Palladino
 
        HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"OK",200,"HTTP/1.1");
        httpResponseHeader->setHeader("Content-Type","text/html"); // Set content type
        // Fetch the date/time string of the last modified attribute and set it to the header
        httpResponseHeader->setHeader("Last-Modified",fileCacheItem->getLastUpdateTime());
        // This implies that the connection terminates after service the request; i.e. keep-alive is not supported
        httpResponseHeader->setHeader("Connection","close");
        httpResponseHeader->setHeader("Content-Length",to_string(fileCacheItem->getSize()));
        httpResponseHeader->respond(); // Write back the response to the client through the TCPSocket
        // Write back the file to the client through the TCPSocket
        p_tcpSocket->writeToSocket(fileCacheItem->getStream(),fileCacheItem->getSize());
        delete (httpResponseHeader); // Delete the HTTP Response
        delete (fileCacheItem); // delete the cache item clone
        return true; // return true
    } 
    catch (HTTPNotFoundExceptionHandler httpNotFoundExceptionHandler)
    { // Exception occurred and caught
        // Handle the exception and send back the corresponding HTTP status response to client
        httpNotFoundExceptionHandler.handle(p_tcpSocket); 
        return false; // return false
    }
}
// Clone a new identical object and return it to the caller
HTTPService * HTTPHTMLService::clone ()
{
    // Instantiate an HTTPHTMLService object and set it up with the same fileCache. 
    // Notice that the clean flag is set to false as the current object will be carrying this out.
    return new HTTPHTMLService(fileCache,false);  
}
HTTPHTMLService::~HTTPHTMLService() // Destructor
{
}
