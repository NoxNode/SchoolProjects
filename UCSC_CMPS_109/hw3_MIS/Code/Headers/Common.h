#pragma once

// common STD and STL includes
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <inttypes.h>
#include <chrono>
#include <thread>

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
#define MAX_FILE_SIZE      3072000
