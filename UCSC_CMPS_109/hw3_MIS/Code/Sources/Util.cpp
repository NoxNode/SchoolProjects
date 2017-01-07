#include "../Headers/Util.h"

void Util::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	string::size_type start_pos = 0;
	try {
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) { // find the next instance of "from"
			str.replace(start_pos, from.size(), to); // replace it with "to"
			start_pos += to.size(); // go to the next position in the string
		}
	}
	catch (...) { // throw error if replace() failed
		throw Error("at line", 0, "failed to replace backslash escaped characters");
	}
}

void Util::splitKeep(string &str, char csplit, vector<string>& out) {
	string curString = ""; // temporary string variable
	char c; // temporary char variable
	char prevC = ' '; // temporary previous char variable
	bool hitDoubleQuote = false; // flag to make sure we handle quotes correctly
	bool hitSingleQuote = false; // flag to make sure we handle quotes correctly
	for (unsigned int i = 0; i < str.size(); i++) { // loop through all characters in the string to split
		c = str[i]; // set c to the char at i of str
		if (c == '\"' && prevC != '\\') { // if we hit a quote and it wasn't escaped with backslash
			if (hitDoubleQuote) { // if we already hit a quote
				out.push_back(curString + c); // push curString
				curString = ""; // reset curString
				hitDoubleQuote = false; // set flag to false
				goto loop; // go to end of loop
			}
			else
				hitDoubleQuote = true; // set flag to true
		}
		else if (c == '\'' && prevC != '\\') { // if we hit a single quote, do the same as above but for single quotes
			if (hitSingleQuote) {
				out.push_back(curString + c);
				curString = "";
				hitSingleQuote = false;
				goto loop;
			}
			else
				hitSingleQuote = true;
		}
		else if (c == ' ' || c == '\t' || c == csplit) { // if we hit whitespace or the char to split by (comma's for this program)
			if (curString != "" && !hitSingleQuote && !hitDoubleQuote) { // if curString isn't empty and we aren't in a quoted string
				out.push_back(curString); // push curString
				curString = ""; // reset curString
			}
			if ((hitSingleQuote || hitDoubleQuote) && (c == ' ' || c == '\t')) // if we hit whitespace withinin a quoted string
				curString += c; // add the whitespace
			if (c != csplit) // if c == whitespace, go to end of loop
				goto loop;
			else if (!hitSingleQuote && !hitDoubleQuote) { // if it's a comma that's not in a quoted string
				out.push_back(","); // add the comma alone as a single element
				goto loop;
			}
		}
		curString += c; // if not whitespace or split char, add it to curString
	loop: // label for things to jump to so we can set prevC and skip adding c
			// this saves a couple lines of code compared to doing prevC = c; then continue; in each case
		prevC = c; // set prevC
	}
	if (curString != "") // if we end on a non-empty string
		out.push_back(curString); // push it
}

Parameter* Util::getNumericConstant(string& str) {
	std::istringstream realStream(str); // stringstream for getting REAL constants
	std::istringstream numericStream(str); // stringstream for getting NUMERIC constants
	bool gotRealVal = true; // flag for if we got a REAL value
	bool gotNumericVal = true; // flag for if we got a NUMERIC value
	char c; // temporary character to see if there's any characters left over
	REAL realValue = 0; // the REAL value of the constant
	NUMERIC numericValue = 0; // the NUMERIC value of the constant
	if (!(realStream >> realValue) || (realStream.get(c))) // if we didn't get the REAL value successfully
		gotRealVal = false; // set the flag to false
	if (!(numericStream >> numericValue) || (numericStream.get(c))) // if we didn't get the NUMERIC value successfully
		gotNumericVal = false; // set the flag to false

	if (gotRealVal && !gotNumericVal) { // if we got the REAL value, but not the NUMERIC, it must be a decimal REAL value
		Parameter* newParam = new Variable<REAL>('R', realValue, true); // allocate the new REAL constant
		return newParam; // return the new parameter
	}
	else if (gotNumericVal) { // if it's not a REAL value, and we got a NUMERIC value
		Parameter* newParam = new Variable<NUMERIC>('N', numericValue, true); // allocate the new NUMERIC constant
		return newParam; // return the new parameter
	}
	return nullptr; // if we didn't return above it's not a NUMERIC or REAL value, so return nullptr
}
