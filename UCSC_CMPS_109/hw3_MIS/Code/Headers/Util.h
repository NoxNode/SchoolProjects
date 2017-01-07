#pragma once

#include "Common.h"
#include "Variable.h"

// Util is the namespace for general utility functions
namespace Util {
	// this function replaces all instances in "str" of "from" with "to"
	// IN str - the str to search in
	// IN from - the string to look for to replace
	// IN to - the string to replace to
	void replaceAll(std::string& str, const std::string& from, const std::string& to);

	// this function splits a line up into a vector of strings containing all the arguments of the line (excludes whitespace)
		// the purpose of this function is to make accessing each arguments easier (and to make sure they are separated by comma's)
	// IN str - the string to split up
	// IN c - the character to split by (for this assignment that is comma's)
	// OUT out - the vector holding the split up strings
	void splitKeep(string &str, char c, vector<string>& out);

	// this function returns a numeric constant variable extracted from a string
	// IN str - the string to extract the numeric constant variable from
	// returns a pointer to a variable on the heap with the numeric value
	Parameter* getNumericConstant(string& str);
}
