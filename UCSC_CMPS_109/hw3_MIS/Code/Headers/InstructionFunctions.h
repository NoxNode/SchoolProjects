#pragma once

#include "Common.h"
#include "Variable.h"

// forward declaration of the Instruction class
class Instruction;

// IFs is the namespace for the InstructionFuntions
namespace IFs {
	// the typdef of InstructionFunction
	typedef void(*InstructionFunction)(Instruction* instruction);

	// all the functions that have the InstructionFunction format
	void ADD(Instruction* instruction);
	void MUL(Instruction* instruction);
	void ASSIGN(Instruction* instruction);
	void OUT(Instruction* instruction);
	void SET_STR_CHAR(Instruction* instruction);
	void GET_STR_CHAR(Instruction* instruction);
	void SLEEP(Instruction* instruction);
	void JMP(Instruction* instruction);
	void JMPZ(Instruction* instruction);
	void JMPNZ(Instruction* instruction);
	void JMPLT(Instruction* instruction);
	void JMPGT(Instruction* instruction);
	void JMPLTE(Instruction* instruction);
	void JMPGTE(Instruction* instruction);
}
