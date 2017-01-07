#include "HTTPXMLService.h"
#include "HTTPResponseHeader.h"
#include "HTTPNotFoundExceptionHandler.h"
// Decode URL encoded string, replace every %ascii code with its equivelant ascii character 
string HTTPXMLService::urldecode(string & urlencoded)
{
    string urldecoded=""; // A string for holding the decoded string
    for ( int i = 0 ;i < urlencoded.length();) 
    { // loop on all characters in the encoded string
        char c = urlencoded[i]; // get the character at the current index
        // check if the character is '%' and there are atleast two characters after it
        if (i < urlencoded.length() - 2 && c == '%') { 
            // Get the next two characters and store them in c1 and c2
            char c1 = urlencoded[i + 1]; 
            char c2 = urlencoded[i + 2];
            // Convert the two characters to lower case
            c1 = tolower(c1);
            c2 = tolower(c2);
            // check if c1 and c2 are hexa digits
            if (isxdigit(c1) && isxdigit(c2)) {
                // convert c1 and c2 to their numeric equivalent
                if (c1 <= '9')      
                    c1 = c1 - '0';
                else
                    c1 = c1 - 'a' + 10;
                if (c2 <= '9')
                    c2 = c2 - '0';
                else
                    c2 = c2 - 'a' + 10;

                c = (char) ((16 * c1) + c2);
                // if the character is a new line replace it with HTML line break
                if ( c == '\n') urldecoded += "<br/>";
                urldecoded+=c; // add the character to the urldecoded string
                i += 3; // increment index by 3: % + 2 characters
            } else { // else it is a normal character
                urldecoded+= c; // add it to the urldecoded string
                i++; // increment the index by 1
            }
        } else { // else this the current character is not '%' or is one of the two last digits
            if (c == '+') c = ' '; // convert the '+' character to white space
            urldecoded+= c; // add it to the urldecoded string
            i++;// increment the index by 1
        }        
    }
    return urldecoded; // return the urldecoded string
}
// Add new form fields to the form_data and the raw_form_data maps
void HTTPXMLService::add_to_form_data(string & form_field)
{
    stringstream iss(form_field); // stringstream for paring the form field( tuple key=value&)
    string field_name = ""; // A string to hold the key
    string field_value = ""; // A string to hold the value
    getline(iss,field_name,'='); // parse whatever before '=' and store it in field_name
    getline(iss,field_value,'&'); // parse whatever between '=' and '&' and store it in field_name
    raw_form_data[field_name]=field_value; // store the raw data into the raw_form_data map
    form_data[urldecode(field_name)]=urldecode(field_value); // store the urldecoded data into the form_data map
}
// Parse the body data of a POST request which is expected to be an HTML form data
void HTTPXMLService::parse_data (HTTPRequest * p_httpRequest)
{
    string data  = p_httpRequest->getBody(); // get the HTTPRequest body data
    stringstream iss(data); // stringstream to parse fields delimited by '&'
    string field = ""; // A field string to store different form fields
    for (;!iss.eof();) // while more tokens exists in the stringstream
    {
        getline(iss,field,'&'); // Get token until next '&' or end of stream
        field += "&"; // append '&' to each the parsing of the field further
        add_to_form_data(field); // parse and add the field to the form data maps
    }
}
// compose the reply body from the maps built up of the form field. The reply body is a XML stream containing an XML form.
string HTTPXMLService::compose_reply()
{
    // The reply string contains the XML stream. We compose it step by step
    string reply = "<HTMLForm>"; // start the main HTMLForm tag
    // for_each iterator loop over form_data with lambda function to build the XML form
    for_each (form_data.begin(),form_data.end(),[&reply](const std::pair<string,string>& it) -> bool {
		reply += "<"; // start the form data tag
		reply += std::get<0>(it); // with the name of the form data as the tag name
		reply += ">";
		reply += std::get<1>(it); // put the form data value in between the start and end tags
		reply += "</"; // end the form data tag
		reply += std::get<0>(it); // with the name of the form data as the tag name
		reply += ">";
		return true;
    });
    reply += "</HTMLForm>"; // end the main HTMLForm tag
    return reply; // return reply

}
HTTPXMLService::HTTPXMLService( )
        :HTTPService(NULL,false) {} // Constructor setting data members using initialization list

// Execute  the HTTPFORM service 
bool HTTPXMLService::execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket)
{
    parse_data(p_httpRequest); // parse the request body data
    string reply = compose_reply(); // compose the HTTP reply body
    //Build and set the HTTP response Header fields.
    HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"OK",200,"HTTP/1.1");
	// edit by Mark Palladino
		// changed content type to "application/xml"
    httpResponseHeader->setHeader("Content-Type","application/xml");
    httpResponseHeader->setHeader("Connection","close");
    httpResponseHeader->setHeader("Content-Length",to_string(reply.length()));
    httpResponseHeader->respond(); // Write back the response to the client through the TCPSocket
    // Write back the file to the client through the TCPSocket
    p_tcpSocket->writeToSocket(reply.c_str(),reply.length());
    delete (httpResponseHeader); // Delete the HTTP Response
    return true; // return true
}
// Clone a new identical object and return it to the caller
HTTPService * HTTPXMLService::clone ()
{
    // Instantiate an HTTPHTMLService object and set it up with the same fileCache. 
    // Notice that the clean flag is set to false as the current object will be carrying this out.
    return new HTTPXMLService();  
}
HTTPXMLService::~HTTPXMLService(){} // Destructor
