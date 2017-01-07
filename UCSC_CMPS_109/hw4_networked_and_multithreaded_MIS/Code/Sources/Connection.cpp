#include "../Headers/Connection.h"
#include <errno.h>

//Constructor: Call parent Thread Constructor
Connection::Connection(TCPSocket * p_tcpSocket) :
Thread()
{
	program = new MISProgram(); // we added this - allocate a new program for this thread
	tcpSocket = p_tcpSocket; // Set the TCP socket
}

// Destructor: delete the TCP socket if set
Connection::~Connection(){
	if ( tcpSocket != NULL ) delete (tcpSocket);
	if ( program != NULL ) delete(program); // we added this - delete program
}

// we added this - described in header
TCPSocket* Connection::getSock() {
    return tcpSocket;
}

// we added this - has all the functionality for handling a connection
void* Connection::threadMainBody(void* arg) {
	TCPSocket* sock = ((Connection*)arg)->getSock(); // get the socket
	string contents; // a string to hold the contents of the code files being sent to us
	char buffer[MAX_BYTES]; // packet buffer
	while(true) {
		int status = sock->readFromSocketWithTimeout(buffer, MAX_BYTES - 1, READ_TIMEOUT, 0); // get a line of code
		// if we timed out or encountered other error, print out error and return
		if(status == 0) {
			cout << "timed out when reading from client" << endl;
			// let the client know we arent doing anything more
			sock->writeToSocket("error recieving code from client;;", strlen("error recieving code from client;;"));
			return NULL;
		}
		if(status < 0) {
			perror("error reading from socket: ");
			// let the client know we arent doing anything more
			sock->writeToSocket("error recieving code from client;;", strlen("error recieving code from client;;"));
			return NULL;
        }
		string newLine(buffer); // make a string of the new line
		if(contents.size() + newLine.size() > MAX_FILE_SIZE) {
			cout << "error: max characters of code exceeded" << endl;
			// let the client know we arent doing anything more
			sock->writeToSocket("error recieving code from client;;", strlen("error recieving code from client;;"));
			return NULL;
		}
		size_t semiColonPos = newLine.find(";");
		if (semiColonPos != std::string::npos) {
			newLine[semiColonPos] = ' ';
			contents += newLine; // append it to the programs errBuffer
			break;
		}
		contents += newLine; // append it to the programs errBuffer
	}
	bool errorInitializing = program->errorHappened(); // store if an error happened
	if(!program->errorHappened())
		program->load(contents); // load if no errors initializing
	if(!program->errorHappened())
		program->run(); // run if no errors loading

    // send the errBuffer to the client
	stringstream errStream(*program->getErrBuffer()); // make a stringstream with errBuffer as its contents
	if(errorInitializing) // if error while initializing
		errStream << "error initializing program" << endl; // print it to the error stream
	string nextChunk; // a string for the next chunk of code
	int curIndex = 0; // keeps track of the current index
	const int sendSize = 65000; // the send size
	for(;;) {
		// get next chunk
		nextChunk = errStream.str().substr(curIndex, sendSize);
		// send next chunk
		sock->writeToSocket(nextChunk.c_str(), nextChunk.size());
		// if we didnt fill the nextChunk we reached the end so break
		if(nextChunk.size() != sendSize) break;
		curIndex += sendSize;
	}
	sock->writeToSocket(";", 1); // send semicolon to let client know this is the end of .out output

    // send the outBuffer to the client
	stringstream outStream(*program->getOutBuffer()); // make a stringstream with the outBuffer as its contents
	curIndex = 0; // keeps track of the current index
	for(;;) {
		// get next chunk
		nextChunk = outStream.str().substr(curIndex, sendSize);
		// send next chunk
		sock->writeToSocket(nextChunk.c_str(), nextChunk.size());
		// if we didnt fill the nextChunk we reached the end so break
		if(nextChunk.size() != sendSize) break;
		curIndex += sendSize;
	}
	sock->writeToSocket(";", 1); // send semicolon to let client know this is the end of .err output

	return NULL;
}
