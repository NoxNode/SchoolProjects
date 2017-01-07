#include "../Headers/Instruction.h"
#include "../Headers/Util.h"
#include "../Headers/MISProgram.h"

Instruction::Instruction(int p_lineNumber, vector<string>& paramStrings, IFs::InstructionFunction p_ptrExecute, TempLoadVars* tempLoadVars, MISProgram* program) :
myProgram(program),
lineNumber(p_lineNumber),
ptrExecute(p_ptrExecute),
JMPFlag(false),
ZFlag(false),
NFlag(false)
{
	// initialize parameter list with paramStrings
    parseParameters(paramStrings, tempLoadVars);
}

void Instruction::parseParameters(vector<string>& paramStrings, TempLoadVars* tempLoadVars) {
	unsigned int i = 1;
	string startOfInstruction = paramStrings[0].substr(0, 3);
    // add index and accumulators for instructions that use arithmetic micro instructions
	// also add them to jumps because most jumps use arithmetic micro instructions and these parameters
	 	// need to be in the same index as they are in ADD, SUB, MUL and DIV
	// we're treating THREAD_BEGIN as a jump because the main thread that spawns the child thread jumps past the thread's code
    if(paramStrings[0] == "ADD" || paramStrings[0] == "SUB" || paramStrings[0] == "MUL" ||
			paramStrings[0] == "DIV" || startOfInstruction == "JMP" || paramStrings[0] == "THREAD_BEGIN") {
        Parameter* index = new Variable<NUMERIC>('N', 0, true);
        parameters.push_back(index); // add it to this instructions parameter list
        Parameter* numericAccumulator = new Variable<NUMERIC>('N', 0, true);
        parameters.push_back(numericAccumulator); // add it to this instructions parameter list
        Parameter* realAccumulator = new Variable<REAL>('R', 0, true);
        parameters.push_back(realAccumulator); // add it to this instructions parameter list
    }
	if(paramStrings[0] == "THREAD_BEGIN") {
		if (paramStrings.size() != 1) // make sure it has no parameters
			throw Error("at line", 0, "expected no parameters");
		// allocate a new variable for the instruciton index of the corresponding THREAD_END for the main thread to jump to
		Parameter* instructionIndex = new Variable<NUMERIC>('N', 0, true);
		parameters.push_back(instructionIndex); // add it to this instructions parameter list
		// allocate a new variable for the instruciton index of this THREAD_BEGIN instruction for the child thread to start at
		Parameter* selfIndex = new Variable<NUMERIC>('N', 0, true);
		parameters.push_back(selfIndex); // add it to this instructions parameter list
		return;
	}
	else if(paramStrings[0] == "THREAD_END" || paramStrings[0] == "BARRIER") {
		if(paramStrings[0] == "BARRIER" && tempLoadVars->inMainThread == false)
			throw Error("at line", tempLoadVars->lineNumber, "BARRIER can only be invoked from the main thread");
		if (paramStrings.size() != 1) // make sure it has no parameters
			throw Error("at line", 0, "expected no parameters");
		return;
	}
	else if(paramStrings[0] == "UNLOCK" || paramStrings[0] == "LOCK") {
		if(tempLoadVars->inMainThread == true) // make sure we arent in the main thread
			throw Error("at line", lineNumber, "(UN)LOCK can only be invoked from a thread");
		if (paramStrings.size() != 2) // make sure it has 1 parameter
			throw Error("at line", 0, "expected variable name");
		// make sure the referenced variable exists
		Parameter* foundVar = (*myProgram->getVariables())[paramStrings[1]];
		if(foundVar == NULL)
			throw Error("at line", 0, "unknown variable: " + paramStrings[1]);
		// see if this lock has been allocated yet
		auto foundLock = tempLoadVars->variableLockIndexes.find(paramStrings[1]);
		if(foundLock == tempLoadVars->variableLockIndexes.end()) { // if not, create and add it
			int mutexIndex = myProgram->getVariableLocks()->size();
			// set the lock index to the index of the mutex to be created
			tempLoadVars->variableLockIndexes[paramStrings[1]] = mutexIndex;
			// set the lock status to 0
			tempLoadVars->variableLockStatus[mutexIndex] = 0;
			// make the mutex
			VariableLock* lock = new VariableLock();
			lock->LOCKersThreadLevel = 0; // init to 0 because no one has locked this lock yet
			if(pthread_mutex_init(&lock->mutex, NULL) != 0) // initialize mutex
				throw Error("at line", 0, "error creating mutex for the variable lock");
			myProgram->getVariableLocks()->push_back(lock);
			// allocate a new variable for the index of the mutex to (un)lock
			Parameter* mutexIndexParam = new Variable<NUMERIC>('N', (NUMERIC)mutexIndex, true);
			parameters.push_back(mutexIndexParam); // add it to this instructions parameter list
		}
		else { // otherwise, it has already been created, so just set the mutexIndexParam to the mutexIndex
			// allocate a new variable for the index of the mutex to (un)lock
			Parameter* mutexIndexParam = new Variable<NUMERIC>('N', (NUMERIC)foundLock->second, true);
			parameters.push_back(mutexIndexParam); // add it to this instructions parameter list
		}
		// allocate a new variable for the thread level of this (un)lock
		Parameter* lockThreadLevel = new Variable<NUMERIC>('N', tempLoadVars->threadLevel, true);
		parameters.push_back(lockThreadLevel);
		return;
	}
	else if (startOfInstruction == "JMP") { // if this is a jump instruction
		if (paramStrings.size() < 2) // make sure it has a label
			throw Error("at line", 0, "need name of label to JMP to");
		// allocate a new variable for the instruciton index this jump instruction "points" to
		Parameter* instructionIndex = new Variable<NUMERIC>('N', 0, true);
		parameters.push_back(instructionIndex); // add it to this instructions parameter list
		// add label outside of the below loop because labels are special (strings not surrounded by quotes and not preceded by $)
		Parameter* labelName = new Variable<STRING>('S', paramStrings[1], true);
		parameters.push_back(labelName); // add the label name to parameters
		i = 2; // skip the label
	}
    // turn the parameters into variables (either reference existing variables or make new ones for constants)
    // parse the parameters
    for(; i < paramStrings.size(); i++) {
		if (paramStrings[i] == ",") // skip comma's
			continue;
		// reference variables
        if(paramStrings[i][0] == '$') { // if this is a variable reference
			Parameter* found = (*myProgram->getVariables())[paramStrings[i]];
			if(found == NULL)
                throw Error("at line", 0, "unknown variable: " + paramStrings[i]);
			parameters.push_back(found);
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
			// make sure it's a string surrounded by double quotes
            if(paramStrings[i].size() < 3 || paramStrings[i][paramStrings[i].size() - 1] != '\"')
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
static void addComparisonJMPMicroInstructions(unsigned int offset, vector<Parameter*>& parameters,
		vector<MIs::MicroInstruction>& microInstructions) {
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
static void addZJMPMicroInstructions(unsigned int offset, vector<Parameter*>& parameters,
		vector<MIs::MicroInstruction>& microInstructions) {
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
	if (parameters[0 + offset]->getIsConstant() == true ||
			(parameters[0 + offset]->getType() != 'R' && parameters[0 + offset]->getType() != 'N'))
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
	// make sure there is a valid number of parameters
		// (+2 for instruction index and label name +3 for arithmetic microInstruction parameters)
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
		// set the only difference between the type checking for these two instructions
		addOnBool = parameters[2]->getIsConstant() == false;
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

MISProgram* Instruction::getProgram() {
	return myProgram;
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
		// only deallocate constants (variables are deallocated by MISProgram)
        if(param != NULL && param->getIsConstant()) {
            delete param;
            parameters[i] = NULL;
        }
    }
}
