#include "DatabaseIterator.h"
#include "main.h"

DatabaseIterator::DatabaseIterator(leveldb_t *db, const string& key) {
	roptions = leveldb_readoptions_create();
	leveldb_readoptions_set_fill_cache(roptions, 0);
	iterator = leveldb_create_iterator(db, roptions);

	leveldb_iter_seek_to_first(iterator);

	const char* cstr_key = key.c_str();
	keyLen = key.length();
	this->key = new char[keyLen];

	for (int i = 0; i < keyLen; ++i) {
		this->key[i] = cstr_key[i];
	}
}

DatabaseIterator::~DatabaseIterator() {
	leveldb_readoptions_destroy(roptions);
	leveldb_iter_destroy(iterator);
	delete key;
}

ILuaObject* g_IterMeta;



LUA_FUNCTION(__tostring) {
	ILuaInterface* g_Lua = Lua();
	DatabaseIterator* iter = (DatabaseIterator*)*g_Lua->GetUserDataPtr(2);
	g_Lua->Push("[database iterator]");
	return 1;
}

LUA_FUNCTION(gc) {
	ILuaInterface* g_Lua = Lua();
	DatabaseIterator* iter = (DatabaseIterator*)*g_Lua->GetUserDataPtr(1);
	delete iter;
	return 0;
}


void DatabaseIterator::initLua(ILuaInterface& g_Lua) {
	println(g_Lua, "DatabaseIterator.cpp initializing.");
	g_IterMeta = g_Lua.GetMetaTable(DB_ITER_TYPE, DB_ITER_ID);
	g_IterMeta->SetMember("__index", g_IterMeta);
	g_IterMeta->SetMember("__tostring", __tostring);
	g_IterMeta->SetMember("__gc", gc);
}