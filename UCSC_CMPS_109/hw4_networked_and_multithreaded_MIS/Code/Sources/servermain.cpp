#include "../Headers/MISProgram.h"
#include "../Headers/TCPServerSocket.h"
#include "../Headers/ThreadManager.h"
#include "../Headers/Connection.h"

int main(int argc, char* argv[]) {
    if(argc != 3) { // 3 because 0 = program name, 1 = ip, 2 = port
		cout << "Usage: server_misvm [ip address] [port]" << endl;
		return 1;
	}
	// get the port from the command line args
	string portStr(argv[2]);
	std::istringstream portStream(portStr); // stringstream for getting the port
	char c; // temporary character to see if there's any characters left over
	long port;
	if (!(portStream >> port) || (portStream.get(c))) { // if we didn't get the port successfully
		cout << "error getting port number from command line argument" << endl;
		return 1;
	}

	// got arguments successfully, so allocate a TCPServerSocket
	TCPServerSocket* sock = new TCPServerSocket(argv[1],port, BACKLOG_SIZE);
	if(!sock->initializeSocket()) { // initialize it
		cout << "error initializing socket" << endl;
		delete sock;
		return 1;
	}
	// allocate a ThreadManager for managing Connection threads
	ThreadManager* threadManager = new ThreadManager();
	while(true) {
		TCPSocket* nextClient = sock->getConnection(); // get the next client connection
		if(nextClient == NULL) { // make sure its valid
			cout << "error getting connection" << endl;
			break;
		}
		try {
			threadManager->cleanup(); // Invoke the thread handler cleaner in case of expired connections
			// Instantiate a new Connection object and pass the returned TCP socket to it
			Connection* conn = new Connection (nextClient);
			conn->start(); // Start the connection thread to communicate with the client
			threadManager->addThread(conn); // Add the connection to the thread handler to be cleaned up when expired
		}
		catch(Error& err) {
			err.out(); // output any errors to std output
		}
	}
	delete threadManager; // deallocate threadManager
	delete sock; // deallocate the server socket

	return 0;
} // end main()
