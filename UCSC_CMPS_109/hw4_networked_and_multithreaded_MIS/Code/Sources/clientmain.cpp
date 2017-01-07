#include "../Headers/MISProgram.h"
#include "../Headers/TCPServerSocket.h"

int main(int argc, char* argv[]) {
    if(argc != 4) { // 4 because 0 = program name, 1 = ip, 2 = port, 3 = file name
		cout << "Usage: client_misvm [ip address] [port] [.mis file name]" << endl;
		return 1;
	}
	// get file name from command line args
	string fileName(argv[3]); // get the file name
	if(fileName.size() < 5) {
		cout << "the file must be at least 5 characters long (including the .mis extension)" << endl;
		return 1;
	}
	string extension = fileName.substr(fileName.size() - 4, 4); // get the extension
	if(extension != ".mis") { // ensure it's .mis
		cout << "the file must have an extension of .mis" << endl;
		return 1;
	}
	MISProgram* program = new MISProgram(fileName); // make a new MISProgram
	if(program->errorHappened()) // output error if error initializing
		cout << "error initializing program" << endl;
	if(!program->errorHappened())
		program->clean(); // clean the .out and .err files if no errors
	if(!program->errorHappened())
		program->load(); // load if no errors
	if(program->errorHappened()) {
		cout << "check .err file" << endl; // tell user to check the .err file if errors
		delete program;
		return 1;
	}
	// successfully loaded, so send it to the server

	// get the port from the command line args
	string portStr(argv[2]);
	std::istringstream portStream(portStr); // stringstream for getting the port
	char c; // temporary character to see if there's any characters left over
	int port;
	if (!(portStream >> port) || (portStream.get(c))) { // if we didn't get the port successfully
		cout << "error getting port number from command line argument" << endl;
		return 1;
	}

	TCPSocket* sock = new TCPSocket(argv[1], port); // connect
	if(sock->isPeerDisconnected()) { // if not connected, print error and return
		cout << "error connecting to server" << endl;
		delete sock;
		delete program;
		return 1;
	}

    ifstream inFile(fileName.c_str()); // open file
    if(!inFile) { // make sure it's open
		cout << "unable to open specified file" << endl;
		delete sock;
		delete program;
		return 1;
	}

	// load file contents into a stringstream
	stringstream contents;
	contents << inFile.rdbuf();
	string nextChunk; // a string for the next chunk of code
	int curIndex = 0; // keeps track of the current index
	const int sendSize = 65000; // the send size
	for(;;) {
		// get next chunk
		nextChunk = contents.str().substr(curIndex, sendSize);
		// send next chunk
		sock->writeToSocket(nextChunk.c_str(), nextChunk.size());
		// if we didnt fill the nextChunk we reached the end so break
		if(nextChunk.size() != sendSize) break;
		curIndex += sendSize;
	}
	inFile.close(); // close the .mis file
	sock->writeToSocket(";", 1); // send semicolon to let server know this is the end of the code

	char buffer[MAX_BYTES]; // packet buffer
	bool hitFirstSemiColon = false;
	while(true) {
		int status = sock->readFromSocket(buffer, MAX_BYTES - 1); // get a line of err output
		// if we encountered an error, print out error and return
		if(status <= 0) {
			perror("error reading from socket: ");
			delete sock;
			delete program;
			return 1;
        }
		string newLine(buffer); // make a string of the new line
		size_t semiColonPos = newLine.find(";"); 
		if (semiColonPos != std::string::npos && !hitFirstSemiColon) {
			string newErrLine = newLine.substr(0, semiColonPos);
			(*program->getErrBuffer()) += newErrLine; // append it to the programs errBuffer
			hitFirstSemiColon = true;
			newLine = newLine.substr(semiColonPos + 1, newLine.size() - semiColonPos - 1);
		}
		semiColonPos = newLine.find(";"); 
		if (semiColonPos != std::string::npos && hitFirstSemiColon) {
			newLine[semiColonPos] = ' ';
			(*program->getOutBuffer()) += newLine; // append it to the programs outBuffer
			break;
		}
		if(!hitFirstSemiColon)
			(*program->getErrBuffer()) += newLine; // append it to the programs errBuffer
		else
			(*program->getOutBuffer()) += newLine; // append it to the programs outBuffer
	}
	program->out(); // output the .out and .err results
	if(program->errorHappened()) {
		cout << "error outputing results" << endl;
		delete sock;
		delete program;
		return 1;
	}
	// no client side errors, so tell user to check .out and .err files
	cout << "check both the .err and .out file" << endl;

	delete sock;
	delete program;
    return 0;
} // end main()
