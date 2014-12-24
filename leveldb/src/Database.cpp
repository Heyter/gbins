#include "Database.h"

inline bool checkError(char* err) {
	bool wasError = err == NULL;
	leveldb_free(&err);
	return !wasError;
}


Database::Database(const char* path) {
	options = leveldb_options_create();
	leveldb_options_set_create_if_missing(options, 1);
	leveldb_options_set_compression(options, leveldb_no_compression);

	char* err = NULL;
	db = leveldb_open(options, path, &err);

	if (err != NULL) {
		err = NULL;
		leveldb_repair_db(options, path, &err);
		if (err != NULL) {
			throw 1;
		}
	}
	if (err) {
		leveldb_free(&err);
	}

	woptions = leveldb_writeoptions_create();
	roptions = leveldb_readoptions_create();

}

Database::~Database() {
	leveldb_close(db);
}

bool Database::remove(const string& key) {
	char* err = NULL;
	
	leveldb_delete(db, woptions, key.c_str(), key.length(), &err);

	if (err != NULL) {
		leveldb_free(&err);
		return false;
	}
	else {
		leveldb_free(&err);
		return true;
	}
}
