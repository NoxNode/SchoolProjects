#include "../Headers/Instruction.h"
#include "../Headers/Util.h"

Instruction::Instruction(int p_lineNumber, vector<string>& paramStrings, IFs::InstructionFunction p_ptrExecute, vector<Parameter*>& variables, string& outFileName) :
lineNumber(p_lineNumber),
ptrExecute(p_ptrExecute),
JMPFlag(false),
ZFlag(false),
NFlag(false)
{
	// initialize parameter list with paramStrings
    parseParameters(paramStrings, variables, outFileName);
}

void Instruction::parseParameters(vector<string>& paramStrings, vector<Parameter*>& variables, string& outFileName) {
    // add index and accumulators for instructions that use arithmetic micro instructions
    if(paramStrings[0] == "ADD" || paramStrings[0] == "SUB" || paramStrings[0] == "MUL" || paramStrings[0] == "DIV" || paramStrings[0].substr(0, 3) == "JMP") {
        Parameter* index = new Variable<NUMERIC>('N', 0, true);
        parameters.push_back(index); // add it to this instructions parameter list
        Parameter* numericAccumulator = new Variable<NUMERIC>('N', 0, true);
        parameters.push_back(numericAccumulator); // add it to this instructions parameter list
        Parameter* realAccumulator = new Variable<REAL>('R', 0, true);
        parameters.push_back(realAccumulator); // add it to this instructions parameter list
    }
    // turn the parameters into variables (either reference existing variables or make new ones for constants)
	unsigned int i = 1;
	if (paramStrings[0].substr(0, 3) == "JMP") { // if this is a jump instruction
		if (paramStrings.size() < 2) // make sure it has a label
			throw Error("at line", 0, "need name of label to JMP to");
		Parameter* instructionIndex = new Variable<NUMERIC>('N', 0, true); // allocate a new variable for the instruciton index this jump instruction "points" to
		parameters.push_back(instructionIndex); // add it to this instructions parameter list
		// add label outside of the below loop because labels are special (strings not surrounded by quotes and not preceded by $)
		Parameter* labelName = new Variable<STRING>('S', paramStrings[1], true);
		parameters.push_back(labelName); // add the label name to parameters
		i = 2; // skip the label
	}
	if (paramStrings[0] == "OUT") { // if this is an OUT instruction
		Parameter* outFile = new Variable<STRING>('S', outFileName, true); // allocate a new string for the output file name
		parameters.push_back(outFile); // add it to this instructions parameters
	}
    // parse the parameters
    for(; i < paramStrings.size(); i++) {
		if (paramStrings[i] == ",") // skip comma's
			continue;
		// reference variables
        if(paramStrings[i][0] == '$') { // if this is a variable reference
            bool found = false; // flag for if the variable was found
            for(unsigned int j = 0; j < variables.size(); j++) {
                if(variables[j]->getName() == paramStrings[i]) { // if we found the variable
                    found = true; // set the found flag
                    parameters.push_back(variables[j]); // add a pointer to the existing variable to parameters
                }
            }
            if(!found) // if it wasn't found, throw an error
                throw Error("at line", 0, "unknown variable: " + paramStrings[i]);
        }
		// allocate character parameter
        else if(paramStrings[i][0] == '\'') { // if we hit a single quote
            if(paramStrings[i].size() != 3 || paramStrings[i][2] != '\'') // make sure it's a character surrounded by single quotes
                throw Error("invalid syntax at line", 0, "invalid character (must be surrounded by single quotes): " + paramStrings[i]);
            else {
                Parameter* newParam = new Variable<CHAR>('C',paramStrings[i][1], true); // allocate the new character constant
                parameters.push_back(newParam); // add it to parameters
            }
        }
		// allocate string parameter
        else if(paramStrings[i][0] == '\"') { // if we hit a double quote
            if(paramStrings[i].size() < 3 || paramStrings[i][paramStrings[i].size() - 1] != '\"') // make sure it's a string surrounded by double quotes
                throw Error("invalid syntax at line", 0, "invalid string (must be surrounded by double quotes): " + paramStrings[i]);
            else {
                STRING value = paramStrings[i].substr(1, paramStrings[i].size() - 2); // extract the actual string
                Parameter* newParam = new Variable<STRING>('S',value, true); // allocate the new string constant
                parameters.push_back(newParam); // add it to parameters
            }
        }
		// allocate numeric constant
        else {
			Parameter* newParam = nullptr;
			if ((newParam = Util::getNumericConstant(paramStrings[i])) == nullptr) // throw an error if we failed to get a numeric constant
				throw Error("at line", 0, "invalid variable name or constant: " + paramStrings[i]);
			parameters.push_back(newParam);
        }
    }
    typeCheckParams(paramStrings[0]);
}

// static function because it's only called in this file (this function is made for code reuse)
static void addComparisonJMPMicroInstructions(unsigned int offset, vector<Parameter*>& parameters, vector<MIs::MicroInstruction>& microInstructions) {
    // add the first value to the accumulator
    if(parameters[0 + offset]->getType() == 'R')
        microInstructions.push_back(MIs::ADDR);
    else
        microInstructions.push_back(MIs::ADDN);
    // subtract the second value from the accumulator
    if(parameters[1 + offset]->getType() == 'R')
        microInstructions.push_back(MIs::SUBR);
    else
        microInstructions.push_back(MIs::SUBN);
    // get the comparison flags
    microInstructions.push_back(MIs::ADDAF);
}

// static function because it's only called in this file (this function is made for code reuse)
static void addZJMPMicroInstructions(unsigned int offset, vector<Parameter*>& parameters, vector<MIs::MicroInstruction>& microInstructions) {
    // add the first value to the accumulator
    if(parameters[0 + offset]->getType() == 'R')
        microInstructions.push_back(MIs::ADDR);
    else
        microInstructions.push_back(MIs::ADDN);
    // get the comparison flags
    microInstructions.push_back(MIs::ADDAF);
}

void Instruction::typeCheckParams(string instructionName) {
	unsigned int offset = 3; // will most likely be replaced by the compiler, just setting this for future extensibility
	int minOffset = 0; // for code reuse
	int maxOffset = 0; // for code reuse
	if (instructionName == "ASSIGN") {
		if (parameters.size() != 2) // make sure there's 2 parameters
			throw Error("at line", 0, "expected 2 parameters");
		if (parameters[0]->getIsConstant() == true) // make sure the first parameter is a variable
			throw Error("at line", 0, "first parameter must be a variable");
		if (parameters[0]->getType() == 'R') {
			// throw error if trying to assign a REAL variable anything other than a REAL value or NUMERIC constant value
			if (parameters[1]->getType() != 'R' && parameters[1]->getType() != 'N')
				throw Error("at line", 0, "cannot assign a REAL variable anything other than a REAL value or a NUMERIC constant value");
		}
		else if (parameters[0]->getType() != parameters[1]->getType())
			throw Error("at line", 0, "parameters must be of the same type");
		return;
	}
	if (instructionName == "ADD") {
		for (unsigned int i = 1 + offset; i < parameters.size(); i++) {
            // for each variable, add to accumulator microInstruction
			if (parameters[i]->getType() == 'R')
				microInstructions.push_back(MIs::ADDR);
			else
				microInstructions.push_back(MIs::ADDN);
		}
        // at the end, set the first variable to the accumulator value
		if (parameters[0 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::ADDAR);
		else
			microInstructions.push_back(MIs::ADDAN);
	}
	else if (instructionName == "SUB") {
        // add the second variable to the accumulator
		if (parameters[1 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::ADDR);
		else
			microInstructions.push_back(MIs::ADDN);
        // subtract the third from the accumulator
		if (parameters[2 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::SUBR);
		else
			microInstructions.push_back(MIs::SUBN);
        // set the first variable to the accumulator value
		if (parameters[0 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::ADDAR);
		else
			microInstructions.push_back(MIs::ADDAN);
	}
	else if (instructionName == "MUL") {
		for (unsigned int i = (unsigned int)(1 + offset); i < parameters.size(); i++) {
            // for each variable, multiply to accumulator microInstruction
			if (parameters[i]->getType() == 'R')
				microInstructions.push_back(MIs::MULR);
			else
				microInstructions.push_back(MIs::MULN);
		}
        // at the end, set the first variable to the accumulator value
		if (parameters[0 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::MULAR);
		else
			microInstructions.push_back(MIs::MULAN);
	}
	else if (instructionName == "DIV") {
        // add the second variable to the accumulator
		if (parameters[1 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::MULR);
		else
			microInstructions.push_back(MIs::MULN);
        // divide the third from the accumulator
		if (parameters[2 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::DIVR);
		else
			microInstructions.push_back(MIs::DIVN);
        // set the first variable to the accumulator value
		if (parameters[0 + offset]->getType() == 'R')
			microInstructions.push_back(MIs::MULAR);
		else
			microInstructions.push_back(MIs::MULAN);
		maxOffset = -10;
	}
	else if (instructionName == "OUT") {
    	if (parameters.size() < 1 || parameters.size() > 12) // make sure there's 1-12 parameters
    		throw Error("at line", 0, "expected at least 1 and at most 12 parameters");
		return;
	}
	else goto skip; // skip over this parameter type check if it's not one of the above instructions
	// make sure there is a valid number of parameters (+4 for indexes and accumulators)
	if (parameters.size() < 3 + offset + minOffset || parameters.size() > 13 + offset + maxOffset)
		throw Error("at line", 0, "invalid number of parameters");
	// make sure the first parameter is a variable of type REAL or NUMERIC
	if (parameters[0 + offset]->getIsConstant() == true || (parameters[0 + offset]->getType() != 'R' && parameters[0 + offset]->getType() != 'N'))
		throw Error("at line", 0, "first parameter must be a variable of type REAL or NUMERIC");
	// make sure all subsequent parameters are of type REAL or NUMERIC
	for (unsigned int i = (unsigned int)(1 + offset); i < parameters.size(); i++) {
		if (parameters[i]->getType() != 'R' && parameters[i]->getType() != 'N')
			throw Error("at line", 0, "all parameters must be of type REAL or NUMERIC");
	}
	return;
skip:
	unsigned int nParams = 1; // for code reuse
	offset = 5;
	if (instructionName == "SLEEP") {
		offset = 0; // no hidden parameters
		nParams = 1; // 1 parameter
	}
	else if (instructionName == "JMP") {
		nParams = 0; // 0 parameters
	}
	else if (instructionName == "JMPZ") {
        // add Z or NZ micro instructions
		addZJMPMicroInstructions(offset, parameters, microInstructions);
		nParams = 1; // 1 parameter
	}
	else if (instructionName == "JMPNZ") {
        // add Z or NZ micro instructions
		addZJMPMicroInstructions(offset, parameters, microInstructions);
		nParams = 1; // 1 parameter
	}
	else if (instructionName == "JMPLT") {
        // add LT,GT,LTE, or GTE micro instructions
		addComparisonJMPMicroInstructions(offset, parameters, microInstructions);
		nParams = 2; // 2 parameters
	}
	else if (instructionName == "JMPGT") {
        // add LT,GT,LTE, or GTE micro instructions
		addComparisonJMPMicroInstructions(offset, parameters, microInstructions);
		nParams = 2; // 2 parameters
	}
	else if (instructionName == "JMPLTE") {
        // add LT,GT,LTE, or GTE micro instructions
		addComparisonJMPMicroInstructions(offset, parameters, microInstructions);
		nParams = 2; // 2 parameters
	}
	else if (instructionName == "JMPGTE") {
        // add LT,GT,LTE, or GTE micro instructions
		addComparisonJMPMicroInstructions(offset, parameters, microInstructions);
		nParams = 2; // 2 parameters
	}
	else goto skip2; // if it's not one of the above instructions skip the below parameter type checking
	// make sure there is a valid number of parameters (+2 for instruction index and label name +3 for arithmetic microInstruction parameters)
	if (parameters.size() != nParams + offset)
		throw Error("at line", 0, "invalid number of parameters");
	// make sure all parameters are of type REAL or NUMERIC
	for (unsigned int i = (unsigned int)(0 + offset); i < parameters.size(); i++) {
		if (parameters[i]->getType() != 'R' && parameters[i]->getType() != 'N')
			throw Error("at line", 0, "all parameters must be of type REAL or NUMERIC");
	}
	return;
skip2:
	bool addOnBool = true; // for code reuse
	if (instructionName == "SET_STR_CHAR") {
		nParams = 1;
	}
	else if (instructionName == "GET_STR_CHAR") {
		addOnBool = parameters[2]->getIsConstant() == false; // set the only difference between the type checking for these two instructions
	}
	if (parameters.size() != 3) // make sure there's 3 parameters
		throw Error("at line", 0, "expected 3 parameters");
	// make sure the first parameter is a variable of type STRING
	if (parameters[0]->getType() != 'S' && parameters[0]->getIsConstant() == false)
		throw Error("at line", 0, "first parameter must be a variable of type STRING");
	// make sure the second parameter is a variable of type REAL or NUMERIC
	if (parameters[1]->getType() != 'R' && parameters[1]->getType() != 'N')
		throw Error("at line", 0, "first parameter must be of type REAL or NUMERIC");
	// make sure the third parameter is of type CHAR
	if (parameters[2]->getType() != 'C' && addOnBool)
		throw Error("at line", 0, "third parameter must be a variable of type CHAR");
}

void Instruction::execute() {
    ptrExecute(this); // execute the function that ptrExecute is pointing to, passing a pointer to this
}

int Instruction::getLineNumber() {
    return lineNumber;
}

vector<Parameter*>* Instruction::getParameters() {
    return &parameters;
}

vector<MIs::MicroInstruction>* Instruction::getMicroInstructions() {
    return &microInstructions;
}

bool Instruction::getJMPFlag() {
    return JMPFlag;
}

void Instruction::setJMPFlag(bool flag) {
    JMPFlag = flag;
}

bool Instruction::getZFlag() {
    return ZFlag;
}

void Instruction::setZFlag(bool flag) {
    ZFlag = flag;
}

bool Instruction::getNFlag() {
    return NFlag;
}

void Instruction::setNFlag(bool flag) {
    NFlag = flag;
}

Instruction::~Instruction() {
    for(unsigned int i = 0; i < parameters.size(); i++) {
        Parameter* param = parameters[i];
        if(param != NULL && param->getIsConstant()) { // only deallocate constants (variables are deallocated by MISProgram)
            delete param;
            parameters[i] = NULL;
        }
    }
}
