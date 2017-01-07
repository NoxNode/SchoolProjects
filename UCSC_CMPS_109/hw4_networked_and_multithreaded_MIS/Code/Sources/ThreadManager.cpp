#include "../Headers/ThreadManager.h"

ThreadManager::ThreadManager() {
    head= tail = NULL; // Constructor sets head and tail to NULL
}

void ThreadManager::addThread(Thread* p_thread) { // Add a new thread
    ThreadNode* node = new ThreadNode(); // we added this
    node->thread = p_thread; // we added this
    node->next = NULL;
    if ( head == NULL ) // If head is null (List empty) point head and tail to the thread
        head = tail = node;
    else { // else add to the end and advance tail
        tail->next = node;
        tail = node;
    }
}

void ThreadManager::cleanup() { // Clean up all finished threads
    ThreadNode* cur = head; // set cur to head
    for ( ; cur!= NULL && !cur->thread->isRunning(); ) { // Loop and delete all finished connection from the front of the list
        cur->thread->waitForRunToFinish(); // Wait for the thread
        ThreadNode* cur1 = cur; // get a pointer to it
        cur = cur->next; // Advance cur
        delete (cur1->thread); // Delete finished thread
        delete (cur1); // Delete finished thread node
    }
    head = cur; // Set head to cur
    if (head == NULL) tail = NULL; // If list is empty set tail to NULL
}

// Invoked upon program termination to Gracefully wait for all running threads to finish
void ThreadManager::terminate() {
    ThreadNode* cur = head; //set cur to jead
    for ( ; cur!= NULL; ) { // Loop on all connections
        cur->thread->waitForRunToFinish(); // wait for the current connection to finish
        if(cur->thread->isRunning())
            cout << "still running" << endl;
        if(cur->thread->isAlive())
            cout << "still alive" << endl;
        ThreadNode* cur1 = cur; // get a pointer to it
        cur = cur->next; // Advance cur
        delete (cur1->thread); // Delete finished thread
        delete (cur1); // Delete finished thread node
    }
    head = tail = NULL; // set head and tail to NULL as the list should be empty
}

ThreadManager::~ThreadManager() {
    terminate(); // Invoke terminate to dispose all connections
}
