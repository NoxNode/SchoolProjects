#pragma once

#include "Common.h"
#include "Variable.h"
#include "Instruction.h"

// the class that manages the loading / parsing and execution of the MIS program
class MISProgram {
private:
	// private data members
	// the .mis .out and .err file names
    string fileName, outFileName, errFileName;

	// the variables vector holds pointers to all of the declared variables of the program
    vector<Parameter*> variables;

	// the variableCloners vector holds pointers to empty objects that are used to instantiate clones
	// the purpose of using a map here is because access is O(log(n))
    map<string, Parameter*> variableCloners;

	// the instructions vector holds pointers to all of the instructions of the program
    vector<Instruction*> instructions;

	// the instructionFunctionPtrs vector holds pointers to all of the different types of functions an instruction might execute
	// this serves a similar purpose to the variableCloners, finding the type of initialization an object should undertake in O(log(n))
    map<string, IFs::InstructionFunction> instructionFunctionPtrs;

	// if any parsing or runtime errors happened, this bool will be set to true
	bool error;

	// this function loads a single line passed to it by the load() function
	// IN lineNumber - the number of the line to be loaded
	// IN line - the line to load as a string
	// IN/OUT doneWithVars - this flag is set once we hit an instruction, and if any variables are declared afterward, throw an error
	// IN/OUT JMPs - a vector of pointers to jump instructions (used for linking jumps to labels)
	// IN/OUT labels - a map containing labels and their corresponding instruction index (not line number, but the index that the instruction
		// is stored at in the instructions vector above
    void loadLine(int lineNumber, string line, bool& doneWithVars, vector<Instruction*>& JMPs, map<string,unsigned int>& labels);

	// this function links JMP instructions to their specified label
	// IN JMPs - a vector of all JMP instructions
	// IN labels - a map of labels and their corresponding instruction index
    void linkJumps(vector<Instruction*>& JMPs, map<string,unsigned int>& labels);
public:
	// the constructor - takes in the .mis file name
    MISProgram(string& p_fileName);

	// this function empties the .err and .out files (to allow everywhere else to append safely)
    void clean();

	// this function loads and parses the .mis file
		// it handles the parsing of parameters to each variable declaration / instruction
		// it handles the allocation and storing of variables into the variables vector
		// it handles the allocation and storing of instructions into the instructions vector
		// it handles the linking of jump instructions
    void load();

	// this function executes each instruction in the instructions vector
		// it does this by calling a wrapper to a private function pointer to the correct instruction to run
    void run();

	// returns if the value of the error data member
	bool errorHappened();

	// the destructor
		// it handles the deallocation of all the variables of the program
		// it handles the deallocation of all the instructions of the program
	virtual ~MISProgram();
};
