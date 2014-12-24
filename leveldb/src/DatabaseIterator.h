#pragma once

#include "leveldb/c.h"
#include <string>
#include <cstring>
#include "ILuaInterface.h"
#include "ILuaModuleManager.h"
#include "LuaUpvalues.h"

#define DB_ITER_TYPE "LevelDbIter"
#define DB_ITER_ID 2392

#pragma comment(lib, "libleveldb.lib")

using namespace std;

extern ILuaObject* g_IterMeta;

class DatabaseIterator {
private:
	leveldb_iterator_t* iterator;
	leveldb_readoptions_t* roptions;
	char* key;
	size_t keyLen;
public:
	DatabaseIterator(leveldb_t *db, const string& key);
	~DatabaseIterator();

	inline void seekFirst() {
		leveldb_iter_seek_to_first(iterator);
	}

	inline void seek(const char* key, size_t length) {
		leveldb_iter_seek(iterator, key, length);
	}

	inline bool valid() {
		return leveldb_iter_valid(iterator) != 0;
	}

	inline void next() {
		leveldb_iter_next(iterator);
	}

	inline const char* getKey(size_t* length) {
		return leveldb_iter_key(iterator, length);
	}

	inline const char* getValue(size_t* length) {
		return leveldb_iter_value(iterator, length);
	}

	// compare the key to the keyprefix of the iterator
	inline bool inRange(const char* key, size_t keyLen) {
		if (keyLen < this->keyLen) {
			return false;
		}

		int diff = memcmp(this->key, key, this->keyLen);
		return diff == 0;
	}

	static void initLua(ILuaInterface& g_Lua);
};
