#include "FileCache.h"
// Default Constructor setting the cache root directory
FileCache::FileCache(string p_root) { root = p_root;
// edit by Mark Palladino
	// added this initialization of the mutex in the constructor
pthread_mutex_init(&mutex, NULL); }
// Get the most updated version of the Cache File Item with p_file_name 
FileCacheItem * FileCache::getFile(string p_file_name)
{
    string file_name = root+p_file_name; // Add the root directory to the file
	// start edit by Mark Palladino
	pthread_mutex_lock(&mutex); // lock mutex (critical section)
	FileCacheItem * fileCacheItem = cache[file_name]; // get the file from cache if there already
    if (fileCacheItem == NULL) { // file does not exist in the cache
        // Create a new file item
        fileCacheItem = new FileCacheItem(file_name);
        cache[file_name] = fileCacheItem; // Add to Map
		pthread_mutex_unlock(&mutex); // unlock mutex
        return fileCacheItem;// return a pointer
    }
	pthread_mutex_unlock(&mutex); // unlock mutex
    return fileCacheItem; // return a pointer to the FileCacheItem object
	// end edit by Mark Palladino
}
FileCache::~FileCache () { // Destructor
    // Loop through iterator to delete all FileCacheItem objects
    for_each (cache.begin(),cache.end(),[](const std::pair<string,FileCacheItem *>& it) -> bool {
            FileCacheItem * fileCacheItem = std::get<1>(it); // Fetch Pair value: FileCacheItem *
            delete(fileCacheItem); // delete FileCacheItem object
            return true; // return true for lambda
       });
	   // edit by Mark Palladino
	       // added this to destroy the mutex in the destructor
	   pthread_mutex_destroy(&mutex);
}
