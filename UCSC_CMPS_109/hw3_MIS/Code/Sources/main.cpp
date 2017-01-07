#include "../Headers/MISProgram.h"

int main(int argc, char* argv[]) {
    if(argc == 2) { // 2 because index 0 has the program name and 1 has the first argument (the file name)
        string fileName(argv[1]); // get the file name
        if(fileName.size() < 5) {
            cout << "the file must be at least 5 characters long (including the .mis extension)" << endl;
            return 0;
        }
        string extension = fileName.substr(fileName.size() - 4, 4); // get the extension
        if(extension != ".mis") { // ensure it's .mis
            cout << "the file must have an extension of .mis" << endl;
            return 0;
        }

        MISProgram* program = new MISProgram(fileName); // allocate a new program
        program->clean(); // clean the .out and .err files
		if (!program->errorHappened()) // if no errors cleaning
			program->load();           // load / parse the .mis program
		if (!program->errorHappened()) // if no errors loading / parsing
			program->run();            // run the program
		if (program->errorHappened())  // if any errors happened
			cout << "check .err file" << endl; // tell user to check the .err file
		else
			cout << "successfully ran " + fileName << endl; // otherwise it was successfully ran

		delete program; // deallocate the program
    }
    else { // if argc != 2 then print error
        cout << "please specify a file name (no spaces allowed)" << endl;
    }
    return 0;
} // end main()
