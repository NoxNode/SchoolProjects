#pragma once

// common STD and STL includes
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <inttypes.h>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/timeb.h>

// using the std namespace
using namespace std;

// typdefs of the possible types of variables
typedef int64_t  NUMERIC;
typedef double   REAL;
typedef char     CHAR;
typedef string   STRING;

// file constraints #defines
#define MAX_STRING_SIZE    256
#define MAX_CHARS_PER_LINE 1024
#define MAX_LINES          2048
#define MAX_FILE_SIZE      MAX_CHARS_PER_LINE * MAX_LINES

#define MAX_BYTES 1024
#define BACKLOG_SIZE 10
#define READ_TIMEOUT 25
