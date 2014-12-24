#pragma once

#include "ILuaInterface.h"
#include "ILuaModuleManager.h"
#include <string>
#include <cstring>
#include "leveldb/c.h"
#pragma comment(lib, "libleveldb.lib")

using namespace std;

class Database {
private:
	leveldb_t *db;
	leveldb_options_t *options;
	leveldb_readoptions_t *roptions;
	leveldb_writeoptions_t *woptions;
public:
	Database(const char* path);
	~Database();

	inline void put(const char* key, size_t keyLength, const char* value, size_t length, char* err) {
		err = NULL;
		leveldb_put(db, woptions, key, keyLength, value, length, &err);
	}
	bool get(const char* key, size_t keyLength, char** result, size_t& length) {
		char* err = NULL;

		char * res = leveldb_get(db, roptions, key, keyLength, &length, &err);
		*result = res;
		
		if (err != NULL) 
		{
			leveldb_free(&res);
			*result = err;
			length = strlen(err);
			return false;
		}
		
		return true;
	}

	bool remove(const string& key);

	inline leveldb_iterator_t* createIterator() {
		return leveldb_create_iterator(db, roptions);
	}

	// get the internal leveldb
	inline leveldb_t* getDb() {
		return db;
	}

	// free memory
	inline void free(char** mem) {
		if (*mem==NULL) {
			throw 123;
		}
		leveldb_free(mem);
		
		*mem=NULL;
		
	}
};
