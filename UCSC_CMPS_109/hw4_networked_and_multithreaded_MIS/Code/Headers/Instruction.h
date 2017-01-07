#pragma once

#include "Common.h"
#include "Parameter.h"
#include "InstructionFunctions.h"
#include "MicroInstructions.h"
#include "Error.h"

class MISProgram;
struct TempLoadVars;

class Instruction {
private:
	// a pointer to the program this instruction belongs to (for getting the output buffer and the output mutex)
	MISProgram* myProgram;

	// the line number of this instruction
    int lineNumber;

	// a vector of parameters being passed to this instruction
    vector<Parameter*> parameters;

	// the pointer to the instruction function to execute
    IFs::InstructionFunction ptrExecute;

    // a vector of instruction function pointers (for avoiding having to type check at execution time)
    vector<MIs::MicroInstruction> microInstructions;

	// the jmp flag (set by jump functions if the MISProgram should change the instruction index
	bool JMPFlag;
    // the Z and N flags (set by ADDAF)
    bool ZFlag, NFlag;

	// this function initializes the instruction by parsing the vector of parameter strings
	// IN paramStrings - the current line split into parameter strings
    void parseParameters(vector<string>& paramStrings, TempLoadVars* tempLoadVars);

    // make sure the parameters to this instruction are valid
    // also, add the MicroInstructions necessary to perform the InstructionFunction
    void typeCheckParams(string instructionName);
public:
	// the constructor
	// IN lineNumber - the line number of this function
	// IN paramStrings - the current line split into parameter strings
	// IN p_ptrExecute - a pointer to the instruction function to execute
	// IN program - a pointer to the program this instruction belongs to
    Instruction(int p_lineNumber, vector<string>& paramStrings, IFs::InstructionFunction p_ptrExecute, TempLoadVars* tempLoadVars, MISProgram* program);

	// this function is a wrapper for executing the function pointed to by ptrExecute
    void execute();

	// returns the line number of this instruction
    int getLineNumber();

	// returns the program this instruction belongs to
	MISProgram* getProgram();

	// returns this instruction's parameters
    vector<Parameter*>* getParameters();

	// returns this instruction's microInstructions
    vector<MIs::MicroInstruction>* getMicroInstructions();

	// returns this instruction's jump flag
    bool getJMPFlag();
	// sets this instructions jump flag
    void setJMPFlag(bool flag);

	// returns this instruction's Z flag
    bool getZFlag();
	// sets this instructions Z flag
    void setZFlag(bool flag);

	// returns this instruction's N flag
    bool getNFlag();
	// sets this instructions N flag
    void setNFlag(bool flag);

	// the destructor
		// it handles deallocation of constants from the parameter list
		// it handles deallocation of the parameter list itself
    virtual ~Instruction();
};
