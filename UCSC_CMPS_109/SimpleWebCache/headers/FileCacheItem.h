#ifndef FILECACHEITEM_H
#define FILECACHEITEM_H

#include "includes.h"
// A FileCacheItem that encapsulates all the file aattributes including content.
// One of the most important things is that an object of that type represent the most receint content of the file.
class FileCacheItem
{
    private:
        string file_name; // The file name
        char * cache_item_stream; // The file content
        long cache_item_size; // The file size
        // A stats object representing the file stats including the last modification date
        struct stat cache_item_stat; 
        char time_string[100]; // A character array to store last update date string
        void load (); // Load file content from disk to cache_item_stream
        pthread_mutex_t mutex;  // A mutex for locking while serving fetch content
        pthread_mutex_t assign_mutex; // A mutex for locking while executing the assignment operator
    public:
        FileCacheItem(); // Default Constructor
        // Constructor setting and loading file attributes
        FileCacheItem(string p_file_name);
        FileCacheItem * fetchContent(); // return a copy of the FileCacheItem object with the latest content
        char * getLastUpdateTime (); // return the last update date string
        char * getStream(); // Return a pointer to the current stream
        long getSize(); // Return the size of the target file.
        void operator=(const FileCacheItem & p_fileCacheItem); // Assignment Operator     
        ~FileCacheItem(); // Destructor
};

#endif // FILECACHEITEM_H