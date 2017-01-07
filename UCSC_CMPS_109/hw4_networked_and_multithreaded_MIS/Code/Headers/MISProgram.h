#pragma once

#include "Common.h"
#include "Variable.h"
#include "Instruction.h"
#include "ThreadManager.h"

// this struct holds all of the temporary variables necessary for loading the .mis program
struct TempLoadVars {
	// the variableCloners vector holds pointers to empty objects that are used to instantiate clones
	// the purpose of using a map here is because access is O(log(n))
	map<string, Parameter*> variableCloners;
	// the instructionFunctionPtrs vector holds pointers to all of the different types of functions an instruction might execute
	// this serves a similar purpose to the variableCloners, finding the type of initialization an object should undertake in O(log(n))
    map<string, IFs::InstructionFunction> instructionFunctionPtrs;

	// the line number of the line currently being loaded
	int lineNumber;
	// temporary string variable to hold each line
	string line;
	// the flag that makes sure no variables are declared below an instruction
	bool doneWithVars;
	// the labels map to hold all labels and their instruction index for linking
	map<string,unsigned int> labels;
	// a vector to hold the names of all the linkable instructions (JMPs, THREAD_BEGIN and THREAD_END and LOCK and UNLOCK)
	vector<Instruction*> linkables;
	// a vector to hold the index of all linkable instructions
	vector<int> linkableIndexes;
	// a vector to hold the names of all the linkable instructions
	vector<string> linkableNames;
	// an int to tell which thread we're in
	int threadLevel;
	// a bool to tell if we're in the main thread
	bool inMainThread;
	// the labelThreadLevels map to hold the thread level of each label
		// this is used to make sure one isn't jumping to code that doesn't belong to the jumper thread
	map<string,int> labelThreadLevels;
	// variableLockIndexes holds the index for a variableLock
	map<string,int> variableLockIndexes;
	// variableLockStatus holds the integer status code for a variableLock
		// 0 = have not encountered a lock or unlock for this variableLock
		// 1 = encountered a lock, but not an unlock
		// 2 = encountered an unlock, but not a lock
		// 3 = encountered a lock and an unlock
	map<int,int> variableLockStatus;
};

// this struct holds the necessary variables for a variable lock
struct VariableLock {
	pthread_mutex_t mutex; // the mutex lock
	// the thread level of the thread that LOCKed this mutex last
	// 0 means it hasnt been locked yet, anything else corresponds to the thread level
	int LOCKersThreadLevel;
};

// the class that manages the loading / parsing and execution of the MIS program
class MISProgram {
private:
	// private data members
	// the .mis .out and .err file names
    string fileName, outFileName, errFileName;

	// the .out and .err buffers
	string outBuffer, errBuffer;

	// the .out and .err buffer mutexes
	pthread_mutex_t outBufferMutex, errBufferMutex;

	// a thread handler to BARRIER/waitOn/terminate child threads spawned by THREAD_BEGIN
	ThreadManager* threadManager;

	// a vector to hold all of the mutexes for locking on variables
	vector<VariableLock*> variableLocks;

	// the variables vector holds pointers to all of the declared variables of the program
    map<string, Parameter*> variables;

	// the instructions vector holds pointers to all of the instructions of the program
    vector<Instruction*> instructions;

	// if any parsing or runtime errors happened, this bool will be set to true
	bool error;

	// this function loads a single line passed to it by the load() function
    void loadLine(TempLoadVars* tempLoadVars);

	// this function links JMP instructions to their specified label
	// it also links THREAD_BEGIN to the corresponding THREAD_END
	// it also makes sure there is at least 1 lock for every unlock and vise versa
	// it also makes sure JMPs dont jump outside of the thread they jump from
    void link(TempLoadVars* tempLoadVars);

	// this function holds the common functionality for load() and load(string&)
	template <typename T>
	void commonLoad(T& inFile, bool outToBuffer);
public:
	// the default constructor - called by a Connection on the server
    MISProgram();
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

	// this function is the same as the above function, except all the file contents is given
    void load(string& fileContents);

	// this function executes each instruction in the instructions vector
		// it does this by calling a wrapper to a private function pointer to the correct instruction to run
    void run();

	// this function outputs the output buffers to the local .out and .err files
	void out();

	// returns if the value of the error data member
	bool errorHappened();

	// returns a pointer to the output buffer
	string* getOutBuffer();

	// returns a pointer to the output buffer mutex
	pthread_mutex_t* getOutBufferMutex();

	// returns a pointer to the output buffer
	string* getErrBuffer();

	// returns a pointer to the output buffer mutex
	pthread_mutex_t* getErrBufferMutex();

	// returns the variables map
	map<string, Parameter*>* getVariables();

	// returns the instructions vector
	vector<Instruction*>* getInstructions();

	// returns the variableLocks vector
	vector<VariableLock*>* getVariableLocks();

	// returns threadManager
	ThreadManager* getThreadManager();

	// the destructor
		// it handles the deallocation of all the variables of the program
		// it handles the deallocation of all the instructions of the program
	virtual ~MISProgram();
};
