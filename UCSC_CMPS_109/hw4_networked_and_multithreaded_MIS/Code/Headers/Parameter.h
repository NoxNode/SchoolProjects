#pragma once

#include "Common.h"
#include "Error.h"

class Parameter {
protected:
	// the first character of the type of this parameter
	// the first character instead of the whole string because it's faster to compare
	char type;

	// a flag determining whether this parameter is constant or not
	bool isConstant;
public:
	// default constructor
    Parameter();

	// a templated function to downcast and get the value from the Variable object this is actually pointing to
    template <typename T>
    T& getValue();

	// a templated function to downcast and set the value of the Variable object this is actually pointing to
	template <typename T>
    void setValue(T p_value);

	// returns the type
    char getType();

	// returns the isConstant flag
	bool getIsConstant();

	// a pure virtual method for initializing this variable
    virtual void initialize(vector<string>& paramStrings) = 0;

	// a pure virtual method for cloning this variable
	virtual Parameter* clone(vector<string>& paramStrings) = 0;

	// virtual destructor
	virtual ~Parameter();
};
