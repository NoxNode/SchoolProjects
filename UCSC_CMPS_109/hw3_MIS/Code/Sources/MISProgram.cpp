#include "../Headers/MISProgram.h"
#include "../Headers/Util.h"

void MISProgram::loadLine(int lineNumber, string line, bool& doneWithVars, vector<Instruction*>& JMPs, map<string,unsigned int>& labels) {
    vector<string> paramStrings; // a vector of strings that contain each parameter and comma separator
    Util::splitKeep(line, ',', paramStrings); // split up the line and store the split strings into paramStrings
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
        throw Error(err.errorCodePrefix, lineNumber, err.message);
    }
	// add label if current line defines a label
    if(paramStrings[0] == "LABEL") {
        if(paramStrings.size() != 2) // make sure it's just "LABEL [label name]"
			throw Error("at line", lineNumber, "expected a label name (label names cannot have spaces)");
        else {
			if (labels.find(paramStrings[1]) != labels.end()) // make sure this label isn't already defined
				throw Error("at line", lineNumber, "duplicate label name not allowed: " + paramStrings[1]);
			// if no errors, set the label to the next instruction index
			labels[paramStrings[1]] = instructions.size();
			return;
        }
    }
    // make sure there are comma's separating parameters
    for(unsigned int i = 2; i < paramStrings.size(); i += 2) {
        if(paramStrings[i] != ",")
            throw Error("invalid syntax at line", lineNumber, "expected , between parameters");
		if(i == paramStrings.size() - 1) // make sure a line doesn't end in a comma
			throw Error("invalid syntax at line", lineNumber, "unexpected ,");
	}
	// handle variable declarations
    if(paramStrings[0] == "VAR") {
        if(doneWithVars) // make sure this variable isn't being delared below an instruction
            throw Error("at line", lineNumber, "cannot declare variables below an instruction");
        if(paramStrings.size() < 6) // make sure there's at least 3 parameters (the 6 is because of the comma's and the VAR string)
            throw Error("invalid syntax at line", lineNumber, "expected at least 3 parameters");

        try {
			// make sure this variable hasn't already been declared
			for(unsigned int i = 0; i < variables.size(); i++) {
				if(variables[i]->getName() == paramStrings[1])
					throw Error("at line", lineNumber, "cannot declare a variable of the same name twice: " + paramStrings[1]);
            }
			// clone the variable of the corresponding type
            Parameter* cloner = variableCloners[paramStrings[3]];
            if(cloner != NULL) { // make sure it's a valid type
                Parameter* newVar = cloner->clone(paramStrings); // instantiate a new variable from paramStrings
                variables.push_back(newVar); // add it to the variables vector
			}
        }
        catch(Error& err) { // catch any errors in the initialization of the variable
			throw Error(err.errorCodePrefix, lineNumber, err.message);
        }
    }
    // handle instructions
    else {
		// set doneWithVars flag
        doneWithVars = true;
		// get the corresponding instruction function pointer
        IFs::InstructionFunction instructionFunctionPtr = instructionFunctionPtrs[paramStrings[0]];
        if(instructionFunctionPtr != NULL) { // if it's a valid instruction
            try {
				// instantiate a new instruction with that instruction function pointer and parameters from paramStrings
                Instruction* newInstruction = new Instruction(lineNumber, paramStrings, instructionFunctionPtr, variables, outFileName);
                instructions.push_back(newInstruction); // add it to the variables vector
                if(paramStrings[0].substr(0, 3) == "JMP") // if it's a jump instruction
                    JMPs.push_back(newInstruction); // add a pointer to it in the JMPs vector
            }
            catch(Error& err) { // catch any errors in the initialization of the instruction
				throw Error(err.errorCodePrefix, lineNumber, err.message);
            }
        }
        else { // if it's not a valid instruction
            throw Error("at line", lineNumber, "unknown instruction: " + paramStrings[0]);
        }
    }
}

void MISProgram::linkJumps(vector<Instruction*>& JMPs, map<string,unsigned int>& labels) {
    for(unsigned int i = 0; i < JMPs.size(); i++) { // loop through jump instructions
        string labelName = (*JMPs[i]->getParameters())[4]->getValue<string>(); // get the label name
        auto iterator = labels.find(labelName); // see if the label name in the instruction matches an actual label name
        if(iterator == labels.end()) // if not, throw an error
            throw Error("at line", JMPs[i]->getLineNumber(), labelName + " is an unknown label");
		// otherwise set the instruction index parameter of the instruction to the instruction index of the label
        unsigned int instructionIndex = iterator->second;
        (*JMPs[i]->getParameters())[3]->setValue<NUMERIC>((NUMERIC)instructionIndex);
    }
}

MISProgram::MISProgram(string& p_fileName) :
fileName(p_fileName),
error(false)
{
    size_t extensionIndex = fileName.find_last_of("."); // get index of the extension
    outFileName = fileName.substr(0, extensionIndex) + ".out"; // replace it with .out for the out file
    errFileName = fileName.substr(0, extensionIndex) + ".err"; // replace it with .err for the error file

	// add variable cloners
    variableCloners["NUMERIC"] = new Variable<NUMERIC>();
    variableCloners["REAL"] = new Variable<REAL>();
    variableCloners["CHAR"] = new Variable<CHAR>();
    variableCloners["STRING"] = new Variable<STRING>();

	// add instruction function pointers
    instructionFunctionPtrs["ADD"] = IFs::ADD;
    instructionFunctionPtrs["SUB"] = IFs::ADD;
    instructionFunctionPtrs["MUL"] = IFs::MUL;
    instructionFunctionPtrs["DIV"] = IFs::MUL;
    instructionFunctionPtrs["ASSIGN"] = IFs::ASSIGN;
	instructionFunctionPtrs["OUT"] = IFs::OUT;
	instructionFunctionPtrs["SET_STR_CHAR"] = IFs::SET_STR_CHAR;
    instructionFunctionPtrs["GET_STR_CHAR"] = IFs::GET_STR_CHAR;
	instructionFunctionPtrs["SLEEP"] = IFs::SLEEP;
	instructionFunctionPtrs["JMP"] = IFs::JMP;
    instructionFunctionPtrs["JMPZ"] = IFs::JMPZ;
    instructionFunctionPtrs["JMPNZ"] = IFs::JMPNZ;
    instructionFunctionPtrs["JMPGT"] = IFs::JMPGT;
    instructionFunctionPtrs["JMPLT"] = IFs::JMPLT;
    instructionFunctionPtrs["JMPGTE"] = IFs::JMPGTE;
    instructionFunctionPtrs["JMPLTE"] = IFs::JMPLTE;
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
        err.out(errFileName);
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
        err.out(errFileName);
		error = true;
        return;
    }
}

void MISProgram::load() {
	// open file
    ifstream inFile(fileName.c_str());
    if(!inFile) { // if it failed to open, output and error
        Error err("Error", 0, "Failed to open specified file");
        err.out(errFileName);
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
		err.out(errFileName);
		error = true;
		return;
	}
	inFile.seekg(0, ios::beg); // seek back to beginning

    string line; // temprorary string variable to hold each line
    int lineNumber = 1; // line numbers start at 1
    bool doneWithVars = false; // the flag that makes sure no variables are declared below an instruction
    vector<Instruction*> JMPs; // the JMPs vector to hold pointers to all jump instructions for linking
    map<string,unsigned int> labels; // the labels map to hold all labels and their instruction index for linking
    while(getline(inFile, line)) { // get the next line
        // handle files with CRLF line feeds
        if(line.size() >= 1 && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
		try {
			if (line.size() > MAX_CHARS_PER_LINE) { // make sure we don't exceed the max number of characters per line
				ostringstream oss;
				oss << "number of characters on each line should not exceed " << MAX_CHARS_PER_LINE;
				Error err("at line", lineNumber, oss.str());
				err.out(errFileName);
				error = true;
				return;
			}
			if (lineNumber > MAX_LINES) { // make sure we don't exceed the max number of lines
				ostringstream oss;
				oss << "number of lines should not exceed " << MAX_LINES;
				Error err("Error", 4, oss.str());
				err.out(errFileName);
				error = true;
				return;
			}
			// load the line
			loadLine(lineNumber, line, doneWithVars, JMPs, labels);
		}
		catch (Error& err) { // catch and output any errors
			err.out(errFileName);
			error = true;
			//return; // don't return in order to report on multiple errors at once
		}
        lineNumber++;
    }
	// after all variables, labels and instructions have been parsed and allocated, link the jumps to their label
    try {
        linkJumps(JMPs, labels);
    }
    catch(Error& err) {
        err.out(errFileName);
        error = true;
        //return; // don't return in order to report on multiple errors at once
    }
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
            err.out(errFileName);
			error = true;
            return; // return because we don't want to keep running if there's a runtime error
        }
    }
}

bool MISProgram::errorHappened() {
	return error;
}

MISProgram::~MISProgram() {
	// delete all instructions and their constant variables
	for (unsigned int i = 0; i < instructions.size(); i++) {
		Instruction* instruction = instructions[i];
		if (instruction != NULL) {
			delete instruction;
			instructions[i] = NULL;
		}
	}
	// delete all variableCloners
    for(auto iterator : variableCloners) {
        if(iterator.second != NULL) {
            delete iterator.second;
            iterator.second = NULL;
        }
    }
	// delete all variables
    for(unsigned int i = 0; i < variables.size(); i++) {
        Parameter* variable = variables[i];
        if(variable != NULL) {
            delete variable;
            variables[i] = NULL;
        }
    }
}
