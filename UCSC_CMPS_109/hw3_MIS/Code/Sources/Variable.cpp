#include "../Headers/Variable.h"
#include "../Headers/Util.h"

template <typename T>
Variable<T>::Variable()
{
    Parameter::name = ""; // set the parent members
    Parameter::type = ' '; // set the parent members
	Parameter::isConstant = false; // set the parent members
}

// defining each version of this function (necessary if we're separating definition from declaration)
template Variable<NUMERIC>::Variable();
template Variable<REAL>::Variable();
template Variable<CHAR>::Variable();
template Variable<STRING>::Variable();

template <typename T>
Variable<T>::Variable(char p_type, T p_value, bool p_isConstant, string p_name) :
value(p_value)
{
    Parameter::name = name; // set the parent members
    Parameter::type = p_type; // set the parent members
	Parameter::isConstant = p_isConstant; // set the parent members
}

// defining each version of this function (necessary if we're separating definition from declaration)
template Variable<NUMERIC>::Variable(char p_type, NUMERIC p_value, bool p_isConstant, string p_name);
template Variable<REAL>::Variable(char p_type, REAL p_value, bool p_isConstant, string p_name);
template Variable<CHAR>::Variable(char p_type, CHAR p_value, bool p_isConstant, string p_name);
template Variable<STRING>::Variable(char p_type, STRING p_value, bool p_isConstant, string p_name);

template <typename T>
T& Variable<T>::getValue() {
    return value;
}

// defining each version of this function (necessary if we're separating definition from declaration)
template NUMERIC& Variable<NUMERIC>::getValue();
template REAL& Variable<REAL>::getValue();
template CHAR& Variable<CHAR>::getValue();
template STRING& Variable<STRING>::getValue();

template <typename T>
void Variable<T>::setValue(T p_value) {
    value = p_value;
}

// defining each version of this function (necessary if we're separating definition from declaration)
template void Variable<NUMERIC>::setValue(NUMERIC p_value);
template void Variable<REAL>::setValue(REAL p_value);
template void Variable<CHAR>::setValue(CHAR p_value);
template void Variable<STRING>::setValue(STRING p_value);

// specialized template because we need to have specific functionality for each type
template <>
void Variable<NUMERIC>::initialize(vector<string>& paramStrings) {
    if(paramStrings.size() != 6) { // make sure there's 6 parameters
        throw Error("invalid syntax at line", 0, "expected 3 parameters");
    }
    name = paramStrings[1]; // set the name to the first parameter (index 1 because of the VAR)
    type = paramStrings[3][0]; // set the type to the first character of the second parameter (index 3 because of comma's and the VAR)
	Parameter* constantParam = nullptr; // push a pointer to a parameter on the stack
	if ((constantParam = Util::getNumericConstant(paramStrings[5])) == nullptr) // if paramStrings[5] returns an invalid numeric constant throw error
		throw Error("invalid syntax at line", 0, "not a NUMERIC value: " + paramStrings[5]);
	else {
		if (constantParam->getType() == 'R') // if it's a REAL value
			value = (NUMERIC)constantParam->getValue<REAL>(); // set value by casting
		else
			value = constantParam->getValue<NUMERIC>(); // set value
		delete constantParam; // delete the allocated parameter
	}
}

// specialized template because we need to have specific functionality for each type
template <>
void Variable<REAL>::initialize(vector<string>& paramStrings) {
    if(paramStrings.size() != 6) { // make sure there's 6 parameters
        throw Error("invalid syntax at line", 0, "expected 3 parameters");
    }
    name = paramStrings[1]; // set the name to the first parameter (index 1 because of the VAR)
    type = paramStrings[3][0]; // set the type to the first character of the second parameter (index 3 because of comma's and the VAR)
	Parameter* constantParam = nullptr; // push a pointer to a parameter on the stack
	if ((constantParam = Util::getNumericConstant(paramStrings[5])) == nullptr) // if paramStrings[5] returns an invalid numeric constant throw error
		throw Error("invalid syntax at line", 0, "not a REAL value: " + paramStrings[5]);
	else {
		if (constantParam->getType() == 'R') // if it's a REAL value
			value = constantParam->getValue<REAL>(); // set value
		else
			value = (REAL)constantParam->getValue<NUMERIC>(); // set value by casting
		delete constantParam; // delete the allocated parameter
	}
}

// specialized template because we need to have specific functionality for each type
template <>
void Variable<CHAR>::initialize(vector<string>& paramStrings) {
    if(paramStrings.size() != 6) { // make sure there's 6 parameters
        throw Error("invalid syntax at line", 0, "expected 3 parameters");
    }
    name = paramStrings[1]; // set the name to the first parameter (index 1 because of the VAR)
    type = paramStrings[3][0]; // set the type to the first character of the second parameter (index 3 because of comma's and the VAR)
    if(paramStrings[5].size() != 3 || paramStrings[5][0] != '\'' || paramStrings[5][2] != '\'') { // make sure it's a character surrounded by quotes
        throw Error("invalid syntax at line", 0, "expected a character surrounded by single quotes");
    }
    value = paramStrings[5][1]; // set the value to the character
}

// specialized template because we need to have specific functionality for each type
template <>
void Variable<STRING>::initialize(vector<string>& paramStrings) {
    unsigned int valueParamIndex = 0; // index of the value parameter
    unsigned int sizeParamIndex = 0; // index of the size parameter
    if(paramStrings.size() != 6) { // if there's not 6 variables
        if(paramStrings.size() != 8) // make sure there's 8 variables
			throw Error("invalid syntax at line", 0, "expected 3 or 4 parameters");
		valueParamIndex = 7; // set the index of the value parameter
		sizeParamIndex = 5; // set the index of the size parameter
    }
    else
        valueParamIndex = 5; // set the index of the value parameter to 5 if no size parameter is present
    if(sizeParamIndex != 0) { // if there's a size parameter
		Parameter* constantParam = nullptr; // push a pointer to a parameter on the stack
		if ((constantParam = Util::getNumericConstant(paramStrings[sizeParamIndex])) == nullptr) // if paramStrings[sizeParamIndex] returns an invalid numeric constant throw error
			throw Error("invalid syntax at line", 0, "not a NUMERIC value: " + paramStrings[sizeParamIndex]);
		else {
			unsigned int size = 0;
			if (constantParam->getType() == 'R') // if it's a REAL value
				size = (unsigned int)constantParam->getValue<REAL>(); // set size by casting
			else
				size = (unsigned int)constantParam->getValue<NUMERIC>(); // set size by casting
			value.reserve(size); // reserve size
			delete constantParam; // delete the allocated parameter
		}
    }
    name = paramStrings[1]; // set the name to the first parameter (index 1 because of the VAR)
    type = paramStrings[3][0]; // set the type to the first character of the second parameter (index 3 because of comma's and the VAR)

    if(paramStrings[valueParamIndex].size() < 3 || paramStrings[valueParamIndex][0] != '\"' ||
            paramStrings[valueParamIndex][paramStrings[valueParamIndex].size() - 1] != '\"') // make sure it's a string surrounded by double quotes
        throw Error("invalid syntax at line", 0, "expected a string surrounded by double quotes");
    else
        value = paramStrings[valueParamIndex].substr(1, paramStrings[valueParamIndex].size() - 2); // set value to the string without the surrounding quotes
}

template <typename T>
Parameter* Variable<T>::clone(vector<string>& paramStrings) {
    Variable<T>* newVar = new Variable<T>(); // allocate a new variable
    newVar->initialize(paramStrings); // initialize that variable
    return newVar; // return a pointer to it
}

// defining each version of this function (necessary if we're separating definition from declaration)
template Parameter* Variable<NUMERIC>::clone(vector<string>& paramStrings);
template Parameter* Variable<REAL>::clone(vector<string>& paramStrings);
template Parameter* Variable<CHAR>::clone(vector<string>& paramStrings);
template Parameter* Variable<STRING>::clone(vector<string>& paramStrings);

template <typename T>
Variable<T>::~Variable() {}

// defining each version of this function (necessary if we're separating definition from declaration)
template Variable<NUMERIC>::~Variable();
template Variable<REAL>::~Variable();
template Variable<CHAR>::~Variable();
template Variable<STRING>::~Variable();
