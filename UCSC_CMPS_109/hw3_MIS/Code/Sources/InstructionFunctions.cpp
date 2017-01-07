#include "../Headers/InstructionFunctions.h"
#include "../Headers/Instruction.h"

// static function because it's only called in this file (this function is made for code reuse)
static void executeArithmeticMicroInstructions(unsigned int offset, Instruction* instruction, NUMERIC numericAccumulatorInit, REAL realAccumulatorInit) {
	vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	vector<MIs::MicroInstruction>* microInstructions = instruction->getMicroInstructions(); // get the microInstructions
	Parameter* index = (*parameters)[0]; // get the index parameter
	(*parameters)[1]->setValue<NUMERIC>(numericAccumulatorInit); // set the numeric accumulator
	(*parameters)[2]->setValue<REAL>(realAccumulatorInit); // set the real accumulator
	for(unsigned int i = 0; i < (*microInstructions).size(); i++) {
		index->setValue<NUMERIC>(i + offset); // set the index
		(*microInstructions)[i](instruction); // pass the instruction with the updated index to the microInstruction
	}
}

void IFs::ADD(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(4, instruction, 0,0);
}

void IFs::MUL(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(4, instruction, 1,1);
}

void IFs::ASSIGN(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters();
	// the below code assigns the first parameter to the value of the second parameter if they're of the same type
	// except a REAL variable can be assigned a NUMERIC value
	if ((*parameters)[0]->getType() == 'R') {
		if ((*parameters)[1]->getType() == 'R')
			(*parameters)[0]->setValue<REAL>((*parameters)[1]->getValue<REAL>());
		else
			(*parameters)[0]->setValue<REAL>((REAL)((*parameters)[1]->getValue<NUMERIC>()));
	}
	else if ((*parameters)[0]->getType() == 'N')
		(*parameters)[0]->setValue<NUMERIC>((*parameters)[1]->getValue<NUMERIC>());
	else if ((*parameters)[0]->getType() == 'C')
		(*parameters)[0]->setValue<CHAR>((*parameters)[1]->getValue<CHAR>());
	else
		(*parameters)[0]->setValue<STRING>((*parameters)[1]->getValue<STRING>());
}

void IFs::OUT(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters();
	ofstream outFile((*parameters)[0]->getValue<STRING>().c_str(), ios::app); // open out file
	if (outFile.is_open()) { // make sure it opened successfully
		for (unsigned int i = 1; i < parameters->size(); i++) { // loop through and output the value of each parameter
			if ((*parameters)[i]->getType() == 'R')
				outFile << (*parameters)[i]->getValue<REAL>();
			else if ((*parameters)[i]->getType() == 'N')
				outFile << (*parameters)[i]->getValue<NUMERIC>();
			else if ((*parameters)[i]->getType() == 'C')
				outFile << (*parameters)[i]->getValue<CHAR>();
			else if ((*parameters)[i]->getType() == 'S')
				outFile << (*parameters)[i]->getValue<STRING>();
		}
		outFile.close();
	}
	else { // throw error if it failed to open
		throw Error("at line", 0, "Failed to output to .out file");
	}
}

void IFs::SET_STR_CHAR(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters();
	unsigned int index = 0; // the index to set the character of
	if ((*parameters)[1]->getType() == 'R')
		index = (unsigned int)(*parameters)[1]->getValue<REAL>(); // set index to the casted REAL value
	else
		index = (unsigned int)(*parameters)[1]->getValue<NUMERIC>(); // set index to the casted NUMERIC value

	if (index > (*parameters)[0]->getValue<STRING>().size()) // make sure index is within the size of the string
		throw Error("at line", 0, "index is out of bounds");
	else if (index == (*parameters)[0]->getValue<STRING>().size()) { // if index == size append the character
		if ((*parameters)[0]->getValue<STRING>().size() < MAX_STRING_SIZE) // make sure we aren't going over the max string size
			(*parameters)[0]->setValue<STRING>((*parameters)[0]->getValue<STRING>() + (*parameters)[2]->getValue<CHAR>());
		else { // throw error if we're going over the max string size
			ostringstream oss;
			oss << "cannot append character to make string size exceed " << MAX_STRING_SIZE;
			throw Error("at line", 0, oss.str());
		}
	}
	else // otherwise set the character at index to the specified CHAR value
		(*parameters)[0]->getValue<STRING>()[index] = (*parameters)[2]->getValue<CHAR>();
}

void IFs::GET_STR_CHAR(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters();
	unsigned int index = 0; // the index to set the character of
	if ((*parameters)[1]->getType() == 'R')
		index = (unsigned int)(*parameters)[1]->getValue<REAL>(); // set index to the casted REAL value
	else
		index = (unsigned int)(*parameters)[1]->getValue<NUMERIC>(); // set index to the casted NUMERIC value

	if (index >= (*parameters)[0]->getValue<STRING>().size()) // make sure index is within the size of the string
		throw Error("at line", 0, "index is out of bounds");
	// set the CHAR variable to the character at the given index of the STRING variable
	(*parameters)[2]->setValue<CHAR>((*parameters)[0]->getValue<STRING>()[index]);
}

void IFs::SLEEP(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters();
	if ((*parameters)[0]->getType() == 'R') { // if it's a REAL variable
		REAL sleepSeconds = (*parameters)[0]->getValue<REAL>();
		std::this_thread::sleep_for(std::chrono::milliseconds((NUMERIC)(sleepSeconds * 1000))); // cast to unsigned and sleep for the specified seconds
	}
	else { // if it's a NUMERIC variable
		NUMERIC sleepSeconds = (*parameters)[0]->getValue<NUMERIC>();
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepSeconds * 1000)); // cast to unsigned and sleep for the specified seconds
	}
}

void IFs::JMP(Instruction* instruction) {
	instruction->setJMPFlag(true);
}

void IFs::JMPZ(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(5, instruction, 0,0);
	if(instruction->getZFlag())
		instruction->setJMPFlag(true);
}

void IFs::JMPNZ(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(5, instruction, 0,0);
	if(!instruction->getZFlag())
		instruction->setJMPFlag(true);
}

void IFs::JMPLT(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(5, instruction, 0,0);
	if(instruction->getNFlag())
		instruction->setJMPFlag(true);
}

void IFs::JMPGT(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(5, instruction, 0,0);
	if(!instruction->getNFlag() && !instruction->getZFlag())
		instruction->setJMPFlag(true);
}

void IFs::JMPLTE(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(5, instruction, 0,0);
	if(instruction->getNFlag() || instruction->getZFlag())
		instruction->setJMPFlag(true);
}

void IFs::JMPGTE(Instruction* instruction) {
	// execute this instruction's microInstructions
	executeArithmeticMicroInstructions(5, instruction, 0,0);
	if(!instruction->getNFlag() || instruction->getZFlag())
		instruction->setJMPFlag(true);
}
