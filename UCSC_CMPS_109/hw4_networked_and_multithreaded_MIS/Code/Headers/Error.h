#pragma once

#include "Common.h"

// struct to throw exceptions with
struct Error {
	// a prefix to the error code (often "at line")
    const char* errorCodePrefix;

	// the error code of this error (often lineNumber)
    int errorCode;

	// the message associated with this error
    string message;

	// the constructor
    Error(const char* p_errorCodePrefix, int p_errorCode, string p_message);

    // outputs to std out
    void out();

	// this function appends the error output buffer string
    void outToBuffer(string& outBuffer, pthread_mutex_t* errOutMutex);

	// this function appends the error to the specified file
    void outToFile(string& fileName);
};
