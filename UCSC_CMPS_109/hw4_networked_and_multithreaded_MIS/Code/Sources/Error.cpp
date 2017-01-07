#include "../Headers/Error.h"

// initialize everything
Error::Error(const char* p_errorCodePrefix, int p_errorCode, string p_message) :
errorCodePrefix(p_errorCodePrefix),
errorCode(p_errorCode),
message(p_message)
{}

void Error::out() {
    cout << errorCodePrefix << " " << errorCode << ": " << message << endl; // output the error message
}

void Error::outToBuffer(string& outBuffer, pthread_mutex_t* errOutMutex) {
	ostringstream oss; // a string stream to output the error message parts to
	oss << errorCodePrefix << " " << errorCode << ": " << message << endl; // output the error message
	pthread_mutex_lock(errOutMutex); // lock the error output mutex so only 1 thread is outputting at a time
    outBuffer += oss.str(); // append to the outBuffer
	pthread_mutex_unlock(errOutMutex); // lock the error output mutex so only 1 thread is outputting at a time
}

void Error::outToFile(string& fileName) {
    ofstream errFile(fileName.c_str(), ios::app); // open the error file
    if(errFile.is_open()) { // make sure it's open
        errFile << errorCodePrefix << " " << errorCode << ": " << message << endl; // output the error message
		errFile.close(); // close the file
    }
    else { // tell the user if the error file failed to open
        cout << "Failed to output to err file" << endl;
    }
}
