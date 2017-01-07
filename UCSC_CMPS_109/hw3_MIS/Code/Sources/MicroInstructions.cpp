#include "../Headers/MicroInstructions.h"
#include "../Headers/Instruction.h"

void MIs::ADDR(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
    unsigned int index1 = 2; // realAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to ADD by
    // perform the ADD and store it in the parameter at index1
    (*parameters)[index1]->setValue<REAL>((*parameters)[index1]->getValue<REAL>() + (*parameters)[index2]->getValue<REAL>());
}

void MIs::ADDN(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to ADD by
    // perform the ADD and store it in the parameter at index1
    (*parameters)[index1]->setValue<NUMERIC>((*parameters)[index1]->getValue<NUMERIC>() + (*parameters)[index2]->getValue<NUMERIC>());
}

void MIs::ADDAR(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
    unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = 2; // realAccumulatorIndex
    // perform the ADD and store it in the parameter at 3 (the variable to be set)
    (*parameters)[3]->setValue<REAL>((*parameters)[index1]->getValue<NUMERIC>() + (*parameters)[index2]->getValue<REAL>());
}

void MIs::ADDAN(Instruction* instruction) {
	vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = 2; // realAccumulatorIndex
    // perform the ADD and store it in the parameter at 3 (the variable to be set)
	NUMERIC value = (NUMERIC)((*parameters)[index1]->getValue<NUMERIC>() + (*parameters)[index2]->getValue<REAL>());
    (*parameters)[3]->setValue<NUMERIC>(value);
}

void MIs::ADDAF(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = 2; // realAccumulatorIndex
    // perform the ADD and set the Z and N flags
	REAL value = (*parameters)[index1]->getValue<NUMERIC>() + (*parameters)[index2]->getValue<REAL>();
	if(value == 0)
		instruction->setZFlag(true);
	else
		instruction->setZFlag(false);
	if(value < 0)
		instruction->setNFlag(true);
	else
		instruction->setNFlag(false);
}

void MIs::SUBR(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 2; // realAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to SUB by
    // perform the SUB and store it in the parameter at index1
    (*parameters)[index1]->setValue<REAL>((*parameters)[index1]->getValue<REAL>() - (*parameters)[index2]->getValue<REAL>());
}

void MIs::SUBN(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to SUB by
    // perform the SUB and store it in the parameter at index1
    (*parameters)[index1]->setValue<NUMERIC>((*parameters)[index1]->getValue<NUMERIC>() - (*parameters)[index2]->getValue<NUMERIC>());
}

void MIs::MULR(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 2; // realAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to MUL by
    // perform the MUL and store it in the parameter at index1
    (*parameters)[index1]->setValue<REAL>((*parameters)[index1]->getValue<REAL>() * (*parameters)[index2]->getValue<REAL>());
}

void MIs::MULN(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to MUL by
    // perform the MUL and store it in the parameter at index1
    (*parameters)[index1]->setValue<NUMERIC>((*parameters)[index1]->getValue<NUMERIC>() * (*parameters)[index2]->getValue<NUMERIC>());
}

void MIs::MULAR(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = 2; // realAccumulatorIndex
    // perform the ADD and store it in the parameter at index1
    (*parameters)[3]->setValue<REAL>((*parameters)[index1]->getValue<NUMERIC>() * (*parameters)[index2]->getValue<REAL>());
}

void MIs::MULAN(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 1; // numericAccumulatorIndex
    unsigned int index2 = 2; // realAccumulatorIndex
    // perform the ADD and store it in the parameter at index1
    (*parameters)[3]->setValue<NUMERIC>((NUMERIC)((*parameters)[index1]->getValue<NUMERIC>() * (*parameters)[index2]->getValue<REAL>()));
}

void MIs::DIVR(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 2; // realAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to DIV by
    if((*parameters)[index2]->getValue<REAL>() == 0) // make sure we aren't dividing by 0
        throw Error("at line", 0, "dividing by 0");
    // perform the DIV and store it in the parameter at index1
    (*parameters)[index1]->setValue<REAL>((*parameters)[index1]->getValue<REAL>() / (*parameters)[index2]->getValue<REAL>());
}

void MIs::DIVN(Instruction* instruction) {
    vector<Parameter*>* parameters = instruction->getParameters(); // get the parameters
	unsigned int index1 = 2; // realAccumulatorIndex
    unsigned int index2 = (unsigned int)((*parameters)[0]->getValue<NUMERIC>()); // get the index of the parameter to DIV by
    if((*parameters)[index2]->getValue<NUMERIC>() == 0) // make sure we aren't dividing by 0
        throw Error("at line", 0, "dividing by 0");
    // perform the DIV and store it in the parameter at index1
    (*parameters)[index1]->setValue<REAL>((*parameters)[index1]->getValue<REAL>() / (*parameters)[index2]->getValue<NUMERIC>());
}
