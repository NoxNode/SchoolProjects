#include "../Headers/MISProgram.h"
#include "../Headers/Util.h"

void MISProgram::loadLine(TempLoadVars* tempLoadVars) {
    vector<string> paramStrings; // a vector of strings that contain each parameter and comma separator
    Util::splitKeep(tempLoadVars->line, ',', paramStrings); // split up the line and store the split strings into paramStrings
	if(paramStrings.size() == 0) // make sure the line isn't empty
        return;
	// replace escape sequences
    try {
    	for(unsigned int i = 0; i < paramStrings.size(); i++) {
    		Util::replaceAll(paramStrings[i], "\\n", "\n");
    		Util::replaceAll(paramStrings[i], "\\r", "\r");
    		Util::replaceAll(paramStrings[i], "\\t", "\t");
    		Util::replaceAll(paramStrings[i], "\\\\", "\\");
    		Util::replaceAll(paramStrings[i], "\\\'", "\'");
    		Util::replaceAll(paramStrings[i], "\\\"", "\"");
    	}
    }
    catch(Error& err) { // throw any errors that were thrown by replaceAll()
        throw Error(err.errorCodePrefix, tempLoadVars->lineNumber, err.message);
    }
	// add label if current line defines a label
    if(paramStrings[0] == "LABEL") {
        if(paramStrings.size() != 2) // make sure it's just "LABEL [label name]"
			throw Error("at line", tempLoadVars->lineNumber, "expected a label name (label names cannot have spaces)");
		if (tempLoadVars->labels.find(paramStrings[1]) != tempLoadVars->labels.end()) // make sure this label isn't already defined
			throw Error("at line", tempLoadVars->lineNumber, "duplicate label name not allowed: " + paramStrings[1]);
		// if no errors, set the label to the next instruction index
		tempLoadVars->labels[paramStrings[1]] = instructions.size();
		tempLoadVars->doneWithVars = true; // make sure there aren't any variables declared after this label
		// set the label thread level to 0 if in main thread, otherwise set it to the thread level
		tempLoadVars->labelThreadLevels[paramStrings[1]] = tempLoadVars->inMainThread ? 0 : tempLoadVars->threadLevel;
		return;
    }
    // make sure there are comma's separating parameters
    for(unsigned int i = 2; i < paramStrings.size(); i += 2) {
        if(paramStrings[i] != ",")
            throw Error("invalid syntax at line", tempLoadVars->lineNumber, "expected , between parameters");
		if(i == paramStrings.size() - 1) // make sure a line doesn't end in a comma
			throw Error("invalid syntax at line", tempLoadVars->lineNumber, "unexpected ,");
	}
	// handle variable declarations
    if(paramStrings[0] == "VAR") {
        if(tempLoadVars->doneWithVars) // make sure this variable isn't being delared below an instruction
            throw Error("at line", tempLoadVars->lineNumber, "cannot declare variables below an instruction");
        if(paramStrings.size() < 6) // make sure there's at least 3 parameters (the 6 is because of the comma's and the VAR string)
            throw Error("invalid syntax at line", tempLoadVars->lineNumber, "expected at least 3 parameters");

        try {
			// make sure this variable hasn't already been declared
			auto found = variables[paramStrings[1]];
			if(found != NULL)
				throw Error("at line", tempLoadVars->lineNumber, "cannot declare a variable of the same name twice: " + paramStrings[1]);
			// clone the variable of the corresponding type
            Parameter* cloner = tempLoadVars->variableCloners[paramStrings[3]];
            if(cloner != NULL) { // make sure it's a valid type
                Parameter* newVar = cloner->clone(paramStrings); // instantiate a new variable from paramStrings
				variables[paramStrings[1]] = newVar;// add it to variables
			}
        }
        catch(Error& err) { // catch any errors in the initialization of the variable
			throw Error(err.errorCodePrefix, tempLoadVars->lineNumber, err.message);
        }
    }
    // handle instructions
    else {
		// set doneWithVars flag
        tempLoadVars->doneWithVars = true;
		// get the corresponding instruction function pointer
        IFs::InstructionFunction instructionFunctionPtr = tempLoadVars->instructionFunctionPtrs[paramStrings[0]];
        if(instructionFunctionPtr != NULL) { // if it's a valid instruction
            try {
				// instantiate a new instruction with that instruction function pointer and parameters from paramStrings
                Instruction* newInstruction = new Instruction(tempLoadVars->lineNumber,
					paramStrings, instructionFunctionPtr, tempLoadVars, this);
                instructions.push_back(newInstruction); // add it to the instructions vector
				string startOfInstruction = paramStrings[0].substr(0, 3);
                if(startOfInstruction == "JMP" || startOfInstruction == "THR" ||
						startOfInstruction == "LOC" || startOfInstruction == "UNL") {// if it's a linkable instruction
					if(paramStrings[0] == "THREAD_BEGIN") { // if we're entering thread code
						tempLoadVars->threadLevel++; // increment the thread level
						tempLoadVars->inMainThread = false; // set inMainThread to false
					}
					else if(paramStrings[0] == "THREAD_END") // if we're exiting thread code
						tempLoadVars->inMainThread = true; // set inMainThread to true
                    tempLoadVars->linkables.push_back(newInstruction); // add a pointer to it in the linkables vector
					tempLoadVars->linkableIndexes.push_back(instructions.size() - 1);
					if(startOfInstruction == "JMP") // if it's a jump instruction
						tempLoadVars->linkableNames.push_back(startOfInstruction); // add the start of the instruction name
					else // otherwise
						tempLoadVars->linkableNames.push_back(paramStrings[0]); // add the instruction name
				}
            }
            catch(Error& err) { // catch any errors in the initialization of the instruction
				throw Error(err.errorCodePrefix, tempLoadVars->lineNumber, err.message);
            }
        }
        else { // if it's not a valid instruction
            throw Error("at line", tempLoadVars->lineNumber, "unknown instruction: " + paramStrings[0]);
        }
    }
}

void MISProgram::link(TempLoadVars* tempLoadVars) {
	Instruction* lastThreadBegin = NULL; // a pointer to the last THREAD_BEGIN instruction
	tempLoadVars->threadLevel = 0;
	tempLoadVars->inMainThread = true;
    for(unsigned int i = 0; i < tempLoadVars->linkables.size(); i++) { // loop through jump instructions
		string instructionName = tempLoadVars->linkableNames[i]; // the name of the current instruction
		Instruction* instruction = tempLoadVars->linkables[i]; // a pointer to the current instruction
		int instructionIndex = tempLoadVars->linkableIndexes[i]; // the index of the current instruction
		vector<Parameter*>* parameters = instruction->getParameters(); // the parameters of the current instruction

		if(instructionName == "THREAD_BEGIN") {
			tempLoadVars->threadLevel++; // increment the thread level
			tempLoadVars->inMainThread = false; // set inMainThread to false
			(*parameters)[4]->setValue<NUMERIC>((NUMERIC)(instructionIndex + 1)); // set the selfIndex parameter (+1 to skip self)
			lastThreadBegin = instruction;
		}
		else if(instructionName == "THREAD_END") {
			tempLoadVars->inMainThread = true; // set inMainThread to true
			if(lastThreadBegin == NULL)
				throw Error("at line", instruction->getLineNumber(), "expected a THREAD_BEGIN before this THREAD_END");
			// set the last THREAD_BEGIN instructions 3rd parameter to this THREAD_END instruction index
			// + 1 because we want to skip over the THREAD_END instruction
			(*lastThreadBegin->getParameters())[3]->setValue<NUMERIC>((NUMERIC)(instructionIndex + 1));
		}
		else if(instructionName == "LOCK") { // set bit in variableLockStatus (for making sure theres an unlock for this lock)
			unsigned int mutexIndex = (unsigned int)(*parameters)[0]->getValue<NUMERIC>();
			tempLoadVars->variableLockStatus[mutexIndex] = tempLoadVars->variableLockStatus[mutexIndex] | 1; // set first bit
		}
		else if(instructionName == "UNLOCK") {
			unsigned int mutexIndex = (unsigned int)(*parameters)[0]->getValue<NUMERIC>();
			tempLoadVars->variableLockStatus[mutexIndex] = tempLoadVars->variableLockStatus[mutexIndex] | 2; // set second bit
		}
		else if(instructionName == "JMP") {
			string labelName = (*parameters)[4]->getValue<STRING>(); // get the label name
			// see if the label name in the instruction matches an actual label name
			auto iterator = tempLoadVars->labels.find(labelName);
			if(iterator == tempLoadVars->labels.end()) // if not, throw an error
				throw Error("at line", instruction->getLineNumber(), labelName + " is an unknown label");
			// see if the label is in the same thread level as this jump
			auto iterator2 = tempLoadVars->labelThreadLevels.find(labelName);
			int labelThreadLevel = iterator2->second;
			// throw error if the thread levels do not match
			if(labelThreadLevel != tempLoadVars->inMainThread ? 0 : tempLoadVars->threadLevel)
				throw Error("at line", instruction->getLineNumber(),
					labelName + " cannot be jumped to because it is outside the thread that this JMP instruction belongs to");
			// otherwise set the instruction index parameter of the instruction to the instruction index of the label
			unsigned int labelInstructionIndex = iterator->second;
			(*parameters)[3]->setValue<NUMERIC>((NUMERIC)labelInstructionIndex);
		}
    }
	// make sure variable locks have at least 1 lock and 1 unlock associated with it
	for(auto status : tempLoadVars->variableLockStatus) {
		if(status.second != 3) {// if first and second bits arent 1, throw error
			// get the variable name this lock is associated with
			for(auto index : tempLoadVars->variableLockIndexes) { // loop through indexes
				if(index.second == status.first) // when we find a match
					throw Error("error", 10, index.first + " must have at least 1 lock and 1 unlock if any at all");
			}
		}
	}
}

MISProgram::MISProgram() :
fileName(""),
error(false)
{
	threadManager = new ThreadManager();
    size_t extensionIndex = fileName.find_last_of("."); // get index of the extension
    outFileName = fileName.substr(0, extensionIndex) + ".out"; // replace it with .out for the out file
    errFileName = fileName.substr(0, extensionIndex) + ".err"; // replace it with .err for the error file

	if(pthread_mutex_init(&outBufferMutex, NULL) != 0) // initialize outBufferMutex
		error = true;
	if(pthread_mutex_init(&errBufferMutex, NULL) != 0) // initialize errBufferMutex
		error = true;
}

MISProgram::MISProgram(string& p_fileName) :
fileName(p_fileName),
error(false)
{
	threadManager = new ThreadManager();
    size_t extensionIndex = fileName.find_last_of("."); // get index of the extension
    outFileName = fileName.substr(0, extensionIndex) + ".out"; // replace it with .out for the out file
    errFileName = fileName.substr(0, extensionIndex) + ".err"; // replace it with .err for the error file

	if(pthread_mutex_init(&outBufferMutex, NULL) != 0) // initialize outBufferMutex
		error = true;
	if(pthread_mutex_init(&errBufferMutex, NULL) != 0) // initialize errBufferMutex
		error = true;
}

void MISProgram::clean() {
	// write empty string to out file
    ofstream outFile(outFileName.c_str(), ios::out);
    if(outFile.is_open()) {
        outFile << "";
        outFile.close();
    }
    else { // output error if unable to output file
        Error err("Error", 1, "Failed to clear out file");
        err.outToFile(errFileName);
		error = true;
        return;
    }
	// write empty string to err file
    ofstream errFile(errFileName.c_str(), ios::out);
    if(errFile.is_open()) {
        errFile << "";
        errFile.close();
    }
    else { // output error if unable to output to file
        Error err("Error", 2, "Failed to clear err file");
        err.outToFile(errFileName);
		error = true;
        return;
    }
}

template <typename T>
void MISProgram::commonLoad(T& inFile, bool outToBuffer) {
	TempLoadVars tempLoadVars;
	// add variable cloners
    tempLoadVars.variableCloners["NUMERIC"] = new Variable<NUMERIC>();
    tempLoadVars.variableCloners["REAL"] = new Variable<REAL>();
	tempLoadVars.variableCloners["CHAR"] = new Variable<CHAR>();
    tempLoadVars.variableCloners["STRING"] = new Variable<STRING>();
	// add instruction function pointers
    tempLoadVars.instructionFunctionPtrs["ADD"] = IFs::ADD;
    tempLoadVars.instructionFunctionPtrs["SUB"] = IFs::ADD;
    tempLoadVars.instructionFunctionPtrs["MUL"] = IFs::MUL;
    tempLoadVars.instructionFunctionPtrs["DIV"] = IFs::MUL;
    tempLoadVars.instructionFunctionPtrs["ASSIGN"] = IFs::ASSIGN;
	tempLoadVars.instructionFunctionPtrs["OUT"] = IFs::OUT;
	tempLoadVars.instructionFunctionPtrs["SET_STR_CHAR"] = IFs::SET_STR_CHAR;
    tempLoadVars.instructionFunctionPtrs["GET_STR_CHAR"] = IFs::GET_STR_CHAR;
	tempLoadVars.instructionFunctionPtrs["SLEEP"] = IFs::SLEEP;
	tempLoadVars.instructionFunctionPtrs["JMP"] = IFs::JMP;
    tempLoadVars.instructionFunctionPtrs["JMPZ"] = IFs::JMPZ;
    tempLoadVars.instructionFunctionPtrs["JMPNZ"] = IFs::JMPNZ;
    tempLoadVars.instructionFunctionPtrs["JMPGT"] = IFs::JMPGT;
    tempLoadVars.instructionFunctionPtrs["JMPLT"] = IFs::JMPLT;
    tempLoadVars.instructionFunctionPtrs["JMPGTE"] = IFs::JMPGTE;
    tempLoadVars.instructionFunctionPtrs["JMPLTE"] = IFs::JMPLTE;
    tempLoadVars.instructionFunctionPtrs["THREAD_BEGIN"] = IFs::THREAD_BEGIN;
    tempLoadVars.instructionFunctionPtrs["THREAD_END"] = IFs::THREAD_END;
    tempLoadVars.instructionFunctionPtrs["LOCK"] = IFs::LOCK;
    tempLoadVars.instructionFunctionPtrs["UNLOCK"] = IFs::UNLOCK;
    tempLoadVars.instructionFunctionPtrs["BARRIER"] = IFs::BARRIER;
	tempLoadVars.lineNumber = 1; // line numbers start at 1
    tempLoadVars.doneWithVars = false; // the flag that makes sure no variables are declared below an instruction
	tempLoadVars.threadLevel = 0; // start at 0 so the next THREAD_BEGIN sets it to 1
	tempLoadVars.inMainThread = true; // always start in main thread

    while(getline(inFile, tempLoadVars.line)) { // get the next line
        // handle files with CRLF line feeds
        if(tempLoadVars.line.size() >= 1 && tempLoadVars.line[tempLoadVars.line.size() - 1] == '\r')
            tempLoadVars.line = tempLoadVars.line.substr(0, tempLoadVars.line.size() - 1);
		if (tempLoadVars.line.size() > MAX_CHARS_PER_LINE) { // make sure we don't exceed the max number of characters per line
			ostringstream oss;
			oss << "number of characters on each line should not exceed " << MAX_CHARS_PER_LINE;
			Error err("at line", tempLoadVars.lineNumber, oss.str());
			if(outToBuffer)
				err.outToBuffer(errBuffer, &errBufferMutex);
			else
				err.outToFile(errFileName);
			error = true;
			return;
		}
		if (tempLoadVars.lineNumber > MAX_LINES) { // make sure we don't exceed the max number of lines
			ostringstream oss;
			oss << "number of lines should not exceed " << MAX_LINES;
			Error err("Error", 4, oss.str());
			if(outToBuffer)
				err.outToBuffer(errBuffer, &errBufferMutex);
			else
				err.outToFile(errFileName);
			error = true;
			return;
		}
		try {
			// load the line
			loadLine(&tempLoadVars);
		}
		catch (Error& err) { // catch and output any errors
			if(outToBuffer)
				err.outToBuffer(errBuffer, &errBufferMutex);
			else
				err.outToFile(errFileName);
			error = true;
			//return; // don't return in order to report on multiple errors at once
		}
        tempLoadVars.lineNumber++;
    }
    try {
		if(tempLoadVars.inMainThread == false) // if we finish loading and we're outside the main thread
			throw Error("at line", tempLoadVars.lineNumber, "expected THREAD_END"); // the program is missing a THREAD_END
		// after all variables, labels and instructions have been parsed and allocated, link the jumps to their label
        link(&tempLoadVars);
    }
    catch(Error& err) {
		if(outToBuffer)
			err.outToBuffer(errBuffer, &errBufferMutex);
		else
			err.outToFile(errFileName);
        error = true;
        //return; // don't return in order to report on multiple errors at once
    }
	// delete all variableCloners
    for(auto iterator : tempLoadVars.variableCloners) {
        if(iterator.second != NULL) {
            delete iterator.second;
            iterator.second = NULL;
        }
    }
}

// for calling with an ifstream
template void MISProgram::commonLoad(ifstream& inFile, bool outToBuffer);
// for calling with a stringstream
template void MISProgram::commonLoad(stringstream& inFile, bool outToBuffer);

void MISProgram::load() {
	// open file
    ifstream inFile(fileName.c_str());
    if(!inFile) { // if it failed to open, output and error
        Error err("Error", 0, "Failed to open specified file");
        err.outToFile(errFileName);
		error = true;
        return;
    }

	// make sure file size is under MAX_FILE_SIZE
	streampos begin, end;
	begin = inFile.tellg(); // get beginning
	inFile.seekg(0, ios::end); // seek to end
	end = inFile.tellg(); // get end
	if (end - begin > MAX_FILE_SIZE) { // if size exceeds MAX_FILE_SIZE, output error and return
        ostringstream oss;
        oss << "file size should not exceed " << MAX_FILE_SIZE;
		Error err("Error", 3, oss.str());
		err.outToFile(errFileName);
		error = true;
		return;
	}
	inFile.seekg(0, ios::beg); // seek back to beginning

	commonLoad(inFile, false);
}

void MISProgram::load(string& fileContents) {
	stringstream inFile(fileContents); // make a stringstream with fileContents
	commonLoad(inFile, true);
}

void MISProgram::run() {
    for(unsigned int i = 0; i < instructions.size(); i++) { // loop through each instruction
		Instruction* instruction = instructions[i];
		try {
            instruction->execute(); // execute the instruction
            if(instruction->getJMPFlag() == true) { // if the jump flag is set
				// set the index to the instruction's first parameter (the instruction index of the specified label)
				// although need to -1 because it will be incremented on the next iteration of this loop
                i = (unsigned int)((*instruction->getParameters())[3]->getValue<NUMERIC>() - 1);
                instruction->setJMPFlag(false); // set jump flag to false
            }
        }
        catch(Error& err) { // catch and output any errors generated by the instruction at runtime
			err.errorCode = instruction->getLineNumber();
            err.outToBuffer(errBuffer, &errBufferMutex);
			error = true;
            return; // return because we don't want to keep running if there's a runtime error
        }
    }
}

void MISProgram::out() {
    ofstream outFile(outFileName.c_str(), ios::app); // open the error file
    if(outFile.is_open()) { // make sure it's open
        outFile << outBuffer << endl; // output the error message
		outFile.close(); // close the file
    }
    else { // tell the user if the out file failed to open
		error = true;
        cout << "Failed to output to out file" << endl;
    }
    ofstream errFile(errFileName.c_str(), ios::app); // open the error file
    if(errFile.is_open()) { // make sure it's open
        errFile << errBuffer << endl; // output the error message
		errFile.close(); // close the file
    }
    else { // tell the user if the error file failed to open
		error = true;
        cout << "Failed to output to err file" << endl;
    }
}

bool MISProgram::errorHappened() {
	return error;
}

string* MISProgram::getOutBuffer() {
	return &outBuffer;
}

pthread_mutex_t* MISProgram::getOutBufferMutex() {
	return &outBufferMutex;
}

string* MISProgram::getErrBuffer() {
	return &errBuffer;
}

pthread_mutex_t* MISProgram::getErrBufferMutex() {
	return &errBufferMutex;
}

map<string, Parameter*>* MISProgram::getVariables() {
	return &variables;
}

vector<Instruction*>* MISProgram::getInstructions() {
	return &instructions;
}

vector<VariableLock*>* MISProgram::getVariableLocks() {
	return &variableLocks;
}

ThreadManager* MISProgram::getThreadManager() {
	return threadManager;
}

MISProgram::~MISProgram() {
	delete threadManager;
	// delete all instructions and their constant variables
	for (unsigned int i = 0; i < instructions.size(); i++) {
		Instruction* instruction = instructions[i];
		if (instruction != NULL) {
			delete instruction;
			instructions[i] = NULL;
		}
	}
	// delete all variables
	for(auto iterator : variables) {
        if(iterator.second != NULL) {
            delete iterator.second;
            iterator.second = NULL;
        }
	}
	// destroy and delete mutexes
	for (unsigned int i = 0; i < variableLocks.size(); i++) {
		VariableLock* lock = variableLocks[i];
		if (lock != NULL) {
			pthread_mutex_destroy(&lock->mutex);
			delete lock;
			variableLocks[i] = NULL;
		}
	}
	pthread_mutex_destroy(&outBufferMutex);
	pthread_mutex_destroy(&errBufferMutex);
}
