#pragma once

#include "Common.h"
#include "Variable.h"

// forward declaration of the Instruction class
class Instruction;

// IFs is the namespace for the MicroInstructions
namespace MIs {
	// the typdef of MicroInstruction
	typedef void(*MicroInstruction)(Instruction* instruction);

	// below is all the functions that have the MicroInstruction format
	// the R suffix means REAL
	// the N suffix means NUMERIC
	// the AR suffix means join the accumulators together and set the REAL variable parameter to that sum
	// the AN suffix means join the accumulators together and set the NUMERIC variable parameter to that sum
	// the AF suffix means join the accumulators together and set the Z and N flags

    void ADDR(Instruction* instruction);
	void ADDN(Instruction* instruction);
	void ADDAR(Instruction* instruction);
	void ADDAN(Instruction* instruction);
	void ADDAF(Instruction* instruction);

    void SUBR(Instruction* instruction);
    void SUBN(Instruction* instruction);

    void MULR(Instruction* instruction);
	void MULN(Instruction* instruction);
	void MULAR(Instruction* instruction);
	void MULAN(Instruction* instruction);

    void DIVR(Instruction* instruction);
	void DIVN(Instruction* instruction);
}
