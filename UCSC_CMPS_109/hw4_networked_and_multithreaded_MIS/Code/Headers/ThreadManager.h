#pragma once
/*
* given in class (as GarbageCollector)
* we changed the name to ThreadManager because it seemed for fitting for what we added
* we added a ThreadNode struct (reason is explained above the struct declaration)
* we changed the code to reflect this change in how threads are stored
*/

#include "Thread.h"

// we added this - makes it so that for any new thread types we want to make,
    // we dont have to have the getting of the next node code repeated
struct ThreadNode {
    Thread* thread;
    ThreadNode* next;
};

// A class to manage expired TCP connection and dispose them one by one
class ThreadManager
{ // It manages a linked list of connections and points to the beginning of the list via "head" and the end of the list via "tail"
private:
    ThreadNode* head; // Pointer to the first connection in the list
    ThreadNode* tail; // Pointer to the last connection in the list
public:
    ThreadManager();// Constructor
    void addThread(Thread* p_thread); // Add a thread
    void cleanup(); // Perform a clean up by disposing expired connections.
    void terminate (); // Wait for all threads to terminate and cleanup all
    ~ThreadManager(); // Destructor
};
