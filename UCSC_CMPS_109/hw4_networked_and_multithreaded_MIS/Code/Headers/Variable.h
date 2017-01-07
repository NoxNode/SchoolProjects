#pragma once

#include "Common.h"
#include "Parameter.h"

template <typename T>
class Variable : public Parameter {
private:
	// the value of this variable
    T value;
public:
	// default constructor
    Variable();

	// more specific constructor (only called directly when instantiating constants)
    Variable(char p_type, T p_value, bool p_isConstant = false);

	// return the value of this variable
    T& getValue();

	// set the value of this variable
    void setValue(T p_value);

	// implementation of the pure virtual method for initializing this variable from paramStrings
    virtual void initialize(vector<string>& paramStrings);

	// implementation of the pure virtual method for cloning a variable from paramStrings
	virtual Parameter* clone(vector<string>& paramStrings);

	// the destructor
    virtual ~Variable();
};
