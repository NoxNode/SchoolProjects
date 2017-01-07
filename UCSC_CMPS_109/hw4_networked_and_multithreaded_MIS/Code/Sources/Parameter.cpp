#include "../Headers/Variable.h"

Parameter::Parameter() :
type(' '),
isConstant(false)
{}

// need this function to downcast instead of being virtual
// because otherwise we wouldn't be able to specify a type to get
template <typename T>
T& Parameter::getValue() {
    //return dynamic_cast<Variable<T>&>(*this).getValue(); // used this for debugging to make sure we aren't casting invalidly
	return ((Variable<T>&)(*this)).getValue(); // the more performant version
}

// defining each version of this function (necessary if we're separating definition from declaration)
template NUMERIC& Parameter::getValue();
template REAL& Parameter::getValue();
template CHAR& Parameter::getValue();
template STRING& Parameter::getValue();

// need this function to downcast instead of being virtual
// because otherwise we wouldn't be able to specify a type to set
template <typename T>
void Parameter::setValue(T p_value) {
	//dynamic_cast<Variable<T>&>(*this).setValue(p_value); // used this for debugging to make sure we aren't casting invalidly
	((Variable<T>&)(*this)).setValue(p_value); // the more performant version
}

// defining each version of this function (necessary if we're separating definition from declaration)
template void Parameter::setValue(NUMERIC p_value);
template void Parameter::setValue(REAL p_value);
template void Parameter::setValue(CHAR p_value);
template void Parameter::setValue(STRING p_value);

char Parameter::getType() {
	return type;
}

bool Parameter::getIsConstant() {
	return isConstant;
}

Parameter::~Parameter() {}
