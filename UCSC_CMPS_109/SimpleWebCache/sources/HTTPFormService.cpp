#include "HTTPFormService.h"
#include "HTTPResponseHeader.h"
#include "HTTPNotFoundExceptionHandler.h"
// Decode URL encoded string, replace every %ascii code with its equivelant ascii character 
string HTTPFORMService::urldecode(string & urlencoded)
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
void HTTPFORMService::add_to_form_data(string & form_field)
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
void HTTPFORMService::parse_data (HTTPRequest * p_httpRequest)
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
// compose the reply body from the maps built up of the form field. The reply body is a HTML stream containing to HTML tables.
// The first table presents the HTML form fields in their raw format and the second table presents the HTML form fields after URL decoding them
string HTTPFORMService::compose_reply()
{
    // The reply string contains the HTML stream. We compose it step by step
    string reply = "<html><head><title>Form Engine</title></head><body>";
    reply += "<table width='100%' border='1'>";     // First HTML Table
    // for_each iterator loop over raw_form_data with lambda function to build the first HTML table body
    for_each (raw_form_data.begin(),raw_form_data.end(),[&reply](const std::pair<string,string>& it) -> bool {
            reply += "<tr><td>";
            reply += std::get<0>(it);
            reply += "</td><td>";
            reply += std::get<1>(it);
            reply += "</td></tr>";
            return true; 
    });
    reply += "</table><br/>";
    reply += "<table width='100%' border='1'>";    // Second HTML Table
    // for_each iterator loop over form_data with lambda function to build the second HTML table body    
    for_each (form_data.begin(),form_data.end(),[&reply](const std::pair<string,string>& it) -> bool {
            reply += "<tr><td>";
            reply += std::get<0>(it);
            reply += "</td><td>";
            reply += std::get<1>(it);
            reply += "</td></tr>";
            return true; 
    });
    reply += "</table>";
    reply += "</body></html>";
    return reply; // return reply

}
HTTPFORMService::HTTPFORMService( )
        :HTTPService(NULL,false) {} // Constructor setting data members using initialization list

// Execute  the HTTPFORM service 
bool HTTPFORMService::execute(HTTPRequest * p_httpRequest,TCPSocket * p_tcpSocket)
{
    parse_data(p_httpRequest); // parse the request body data
    string reply = compose_reply(); // compose the HTTP reply body
    //Build and set the HTTP response Header fields.
    HTTPResponseHeader * httpResponseHeader = new HTTPResponseHeader(p_tcpSocket,"OK",200,"HTTP/1.1");
    httpResponseHeader->setHeader("Content-Type","text/html");
    httpResponseHeader->setHeader("Connection","close");
    httpResponseHeader->setHeader("Content-Length",to_string(reply.length()));
    httpResponseHeader->respond(); // Write back the response to the client through the TCPSocket
    // Write back the file to the client through the TCPSocket
    p_tcpSocket->writeToSocket(reply.c_str(),reply.length());
    delete (httpResponseHeader); // Delete the HTTP Response
    return true; // return true
}
// Clone a new identical object and return it to the caller
HTTPService * HTTPFORMService::clone ()
{
    // Instantiate an HTTPHTMLService object and set it up with the same fileCache. 
    // Notice that the clean flag is set to false as the current object will be carrying this out.
    return new HTTPFORMService();  
}
HTTPFORMService::~HTTPFORMService(){} // Destructor
