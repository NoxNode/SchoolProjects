#pragma once

#include "Thread.h"
#include "MISProgram.h"

class ChildThread : public Thread {
private:
    // a pointer to the THREAD_BEGIN instruction that spawned this child thread
    Instruction* instruction;
public:
    ChildThread(Instruction* p_instruction); // Constructor: Set instruction
    void* threadMainBody (void* arg); // Main thread body that executes the threads instructions
    Instruction* getInstruction();
    virtual ~ChildThread(); // Destructor
};
