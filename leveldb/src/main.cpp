#define WIN32_LEAN_AND_MEAN
#define NO_SDK

#include <string>
#ifdef _WIN32
	#include <Windows.h>
#endif

#include "main.h"

#include "DatabaseIterator.h"
#include "Vector.h"
#include "Murmer.h"
#include "sha1.h"
#include "UniqueID.h"

using namespace GarrysMod::Lua;

GMOD_MODULE(Init, Shutdown);

void println(ILuaInterface& g_Lua, const char* msg) {
	ILuaObject* print = g_Lua.GetGlobal("print");
	g_Lua.Push(print);
	g_Lua.Push(msg);
	g_Lua.Call(1, 0);
	print->UnReference();
}

Database db("garrysmod/x_leveldb");

LUA_FUNCTION(db_set) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	int type = g_Lua->GetType(2);

	char* err = NULL;


	if (type == Type::STRING) {
		size_t valueLength;
		const char* value = g_Lua->GetString(2, &valueLength);
		db.put(key, keyLength, value, valueLength, err);
	}
	else if (type == Type::NUMBER) {
		double value = g_Lua->GetNumber(2);
		db.put(key, keyLength, (char*)&value, sizeof value, err);
	}
	else if (type == Type::VECTOR) {
		Vector* vec = (Vector*)*g_Lua->GetUserDataPtr(2);
		db.put(key, keyLength, (char*) vec, sizeof(Vector), err);
	}
	else if (type == Type::ANGLE) {
		Angle* ang = (Angle*)*g_Lua->GetUserDataPtr(2);
		db.put(key, keyLength, (char*)ang, sizeof(Angle), err);
	}
	else if (type == Type::BOOL) {
		bool value = g_Lua->GetBool(2);
		db.put(key, keyLength, (char*)&value, sizeof value, err);
	}
	else {
		g_Lua->Error("expected data arg #2, got nil");
	}

	if (err != NULL) {
		g_Lua->Push(false);
		g_Lua->Push(err);
		leveldb_free(&err);
		return 2;
	}
	else {
		leveldb_free(&err);
		g_Lua->Push(true);
		return 1;
	}
}

LUA_FUNCTION(db_setString) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);
	g_Lua->CheckType(2, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* err = NULL;


	size_t valueLength;
	const char* value = g_Lua->GetString(2, &valueLength);
	db.put(key, keyLength, value, valueLength, err);

	if (err != NULL) {
		g_Lua->Push(false);
		g_Lua->Push(err);
		leveldb_free(&err);
		return 2;
	}
	else {
		leveldb_free(&err);
		g_Lua->Push(true);
		return 1;
	}
}

LUA_FUNCTION(db_setDouble) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);
	g_Lua->CheckType(2, Type::NUMBER);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* err = NULL;

	double value = g_Lua->GetNumber(2);
	db.put(key, keyLength, (char*)&value, sizeof(value), err);

	if (err != NULL) {
		g_Lua->Push(false);
		g_Lua->Push(err);
		leveldb_free(&err);
		return 2;
	}
	else {
		leveldb_free(&err);
		g_Lua->Push(true);
		return 1;
	}
}

LUA_FUNCTION(db_setInteger) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);
	g_Lua->CheckType(2, Type::NUMBER);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* err = NULL;

	int value = g_Lua->GetNumber(2);
	db.put(key, keyLength, (char*)&value, sizeof value, err);

	if (err != NULL) {
		g_Lua->Push(false);
		g_Lua->Push(err);
		leveldb_free(&err);
		return 2;
	}
	else {
		//leveldb_free(&err);
		g_Lua->Push(true);
		return 1;
	}
}

LUA_FUNCTION(db_setVector) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);
	g_Lua->CheckType(2, Type::VECTOR);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* err = NULL;

	Vector* vec = (Vector*)*g_Lua->GetUserDataPtr(2);
	db.put(key, keyLength, (char*)vec, sizeof(Vector), err);

	if (err != NULL) {
		g_Lua->Push(false);
		g_Lua->Push(err);
		leveldb_free(&err);
		return 2;
	}
	else {
		leveldb_free(&err);
		g_Lua->Push(true);
		return 1;
	}
}

LUA_FUNCTION(db_setAngle) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);
	g_Lua->CheckType(2, Type::ANGLE);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* err = NULL;

	Angle* ang = (Angle*)*g_Lua->GetUserDataPtr(2);
	db.put(key, keyLength, (char*)ang, sizeof(Angle), err);

	if (err != NULL) {
		g_Lua->Push(false);
		g_Lua->Push(err);
		leveldb_free(&err);
		return 2;
	}
	else {
		leveldb_free(&err);
		g_Lua->Push(true);
		return 1;
	}
}

LUA_FUNCTION(db_setBool) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);
	g_Lua->CheckType(2, Type::ANGLE);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* err = NULL;

	bool value = g_Lua->GetBool(2);
	db.put(key, keyLength, (char*)&value, sizeof value, err);

	if (err != NULL) {
		g_Lua->Push(false);
		g_Lua->Push(err);
		leveldb_free(&err);
		return 2;
	}
	else {
		leveldb_free(&err);
		g_Lua->Push(true);
		return 1;
	}
}

LUA_FUNCTION(db_getString) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* result = NULL;
	size_t length;
	bool success = db.get(key, keyLength, &result, length);

	g_Lua->Push(success);
	g_Lua->Push(result, length);

	leveldb_free(&result);
	
	return 2;
}

LUA_FUNCTION(db_getDouble) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* result = NULL;
	size_t length;
	bool success = db.get(key, keyLength, &result, length);
	
	if (success) {
		if (length == sizeof(double)) {
			double* result_d = (double*) result;
			g_Lua->Push(true);
			g_Lua->Push(*result_d);
		}
		else {
			g_Lua->Push(false);
			g_Lua->Push("value could not be converted to double");
		}
	}
	else {
		g_Lua->Push(false);
		g_Lua->Push(result, length);
	}

	leveldb_free(&result);

	return 2;
}

LUA_FUNCTION(db_getInteger) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* result = NULL;
	size_t length;
	bool success = db.get(key, keyLength, &result, length);

	if (success) {
		if (length == sizeof(int)) {
			int* result_i = (int*)result;
			g_Lua->Push(true);
			g_Lua->Push(*result_i);
		}
		else {
			g_Lua->Push(false);
			g_Lua->Push("value could not be converted to double");
		}
	}
	else {
		g_Lua->Push(false);
		g_Lua->Push(result, length);
	}

	//leveldb_free(&result);

	return 2;
}

LUA_FUNCTION(db_getBool) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* result = NULL;
	size_t length;
	bool success = db.get(key, keyLength, &result, length);

	if (success) {
		if (length == sizeof(bool)){
			bool* result_b = (bool*)result;
			g_Lua->Push(true);
			g_Lua->Push(*result_b);
		}
		else {
			g_Lua->Push(false);
			g_Lua->Push("value could not be converted to bool");
		}
	}
	else {
		g_Lua->Push(false);
		g_Lua->Push(result, length);
	}

	leveldb_free(&result);

	return 2;
}

LUA_FUNCTION(db_getVector) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* result = NULL;
	size_t length;
	bool success = db.get(key, keyLength, &result, length);

	if (success) {
		if (length == sizeof(Vector)) {
			Vector* result_v = (Vector*)result;

			g_Lua->Push(true);

			ILuaObject* newVector = g_Lua->GetGlobal("Vector");
			g_Lua->Push(newVector);
			g_Lua->Push(result_v->x);
			g_Lua->Push(result_v->y);
			g_Lua->Push(result_v->z);
			g_Lua->Call(3, 1);
			newVector->UnReference();
		}
		else {
			g_Lua->Push(false);
			g_Lua->Push("value could not be converted to Vector");
		}
	}
	else {
		g_Lua->Push(false);
		g_Lua->Push(result, length);
	}

	leveldb_free(&result);

	return 2;
}

LUA_FUNCTION(db_getAngle) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t keyLength;
	const char* key = g_Lua->GetString(1, &keyLength);

	char* result = NULL;
	size_t length;
	bool success = db.get(key, keyLength, &result, length);

	if (success) {
		if (length == sizeof(Angle)) {
			Angle* result_v = (Angle*)result;

			g_Lua->Push(true);

			ILuaObject* newAngle = g_Lua->GetGlobal("Angle");
			g_Lua->Push(newAngle);
			g_Lua->Push(result_v->p);
			g_Lua->Push(result_v->y);
			g_Lua->Push(result_v->r);
			g_Lua->Call(3, 1);
			newAngle->UnReference();
		}
		else {
			g_Lua->Push(false);
			g_Lua->Push("value could not be converted to Angle");
		}
	}
	else {
		g_Lua->Push(false);
		g_Lua->Push(result, length);
	}

	leveldb_free(&result);

	return 2;
}

LUA_FUNCTION(database_delete) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	string key = g_Lua->GetString(1);
	bool success = db.remove(key);

	g_Lua->Push(success);
	return 1;
}

/*
	ITERATING
*/
LUA_FUNCTION(fn_iter) {
	ILuaInterface* g_Lua = Lua();

	DatabaseIterator* iter = (DatabaseIterator*)*g_Lua->GetUserDataPtr(lua_upvalueindex(1));
	
	if (iter->valid()) { // check we haven't hit the end of the database
		size_t length;

		const char* key = iter->getKey(&length);
		
		if (iter->inRange(key, length)) { // kill iteration if we've walked off the key range
			g_Lua->Push(key, length);
			const char* value = iter->getValue(&length);
			g_Lua->Push(value, length);

			iter->next();
			return 2;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}

	return 1;
}

LUA_FUNCTION(new_iter) {
	ILuaInterface* g_Lua = Lua();
	LUA->CheckType(1, Type::STRING);

	string key = LUA->GetString(1);
	
	g_Lua->Push(key.c_str(), key.length());
	LUA->PushCClosure(fn_iter, 1);

	DatabaseIterator* iter = new DatabaseIterator(db.getDb(), key);
	iter->seek(key.c_str(), key.length());

	ILuaObject* iter_obj = g_Lua->NewUserData(g_IterMeta);
		iter_obj->SetUserData(iter, DB_ITER_ID);
		g_Lua->Push(iter_obj);
	iter_obj->UnReference();

	LUA->PushCClosure(fn_iter, 1);

	return 1;
}

/*
STRING CONVERSIONS
*/
LUA_FUNCTION(strToDouble) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t length;
	const char* val = g_Lua->GetString(1, &length);

	if (length == sizeof(double)){
		double* val_d = (double*) val;
		g_Lua->Push(*val_d);
		return 1;
	}
	else {
		return 0;
	}
}

LUA_FUNCTION(strToInteger) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t length;
	const char* val = g_Lua->GetString(1, &length);

	if (length == sizeof(int)){
		const int* val_i = (const int*) val;
		g_Lua->Push(*val_i);
		return 1;
	}
	else {
		return 0;
	}
}

LUA_FUNCTION(strToVector) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t length;
	const char* val = g_Lua->GetString(1, &length);

	if (length == sizeof(Vector)){
		const Vector* val_v = (const Vector*)val;
		ILuaObject* newVector = g_Lua->GetGlobal("Vector");
		g_Lua->Push(newVector);
		g_Lua->Push(val_v->x);
		g_Lua->Push(val_v->y);
		g_Lua->Push(val_v->z);
		g_Lua->Call(3, 1);
		newVector->UnReference();
		return 1;
	}
	else {
		return 0;
	}
}


LUA_FUNCTION(strToAngle) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t length;
	const char* val = g_Lua->GetString(1, &length);

	if (length == sizeof(Angle)){
		const Angle* val_a = (const Angle*)val;
		ILuaObject* newAngle = g_Lua->GetGlobal("Angle");
		g_Lua->Push(newAngle);
		g_Lua->Push(val_a->p);
		g_Lua->Push(val_a->y);
		g_Lua->Push(val_a->r);
		g_Lua->Call(3, 1);
		newAngle->UnReference();
		return 1;
	}
	else {
		return 0;
	}
}

LUA_FUNCTION(strToBool) {
	ILuaInterface* g_Lua = Lua();
	g_Lua->CheckType(1, Type::STRING);

	size_t length;
	const char* val = g_Lua->GetString(1, &length);

	if (length == sizeof(bool)){
		bool* val_b = (bool*) val;
		g_Lua->Push(*val_b);
		return 1;
	}
	else {
		return 0;
	}
}


int Init(lua_State *L)
{
	ILuaInterface* g_Lua = Lua();
	ILuaBase* g_LuaBase = L->luabase;

	println(*g_Lua, "================================================");
	println(*g_Lua, "== Loaded LevelDB interface by thelastpenguin ==");
	println(*g_Lua, "== Running version 1.0.0                      ==");
	println(*g_Lua, "== - thanks to ;Meepen™ for C++ knoledge      ==");
	println(*g_Lua, "================================================");
	println(*g_Lua, "     Compiled on... " __DATE__ ", " __TIME__ "");

	DatabaseIterator::initLua(*g_Lua);
	UniqueID::initLua(*g_Lua);


	ILuaObject* funcTable = g_Lua->GetNewTable();

	funcTable->SetMember("set", db_setString);
	funcTable->SetMember("get", db_getString);
	
	funcTable->SetMember("setAny", db_set);

	funcTable->SetMember("setString",	db_setString);
	funcTable->SetMember("setDouble",	db_setDouble);
	funcTable->SetMember("setVector",	db_setVector);
	funcTable->SetMember("setAngle",	db_setAngle);
	funcTable->SetMember("setBool",		db_setBool);
	funcTable->SetMember("setInteger",	db_setInteger);

	funcTable->SetMember("getDouble",	db_getDouble);
	funcTable->SetMember("getString",	db_getString);
	funcTable->SetMember("getVector",	db_getVector);
	funcTable->SetMember("getAngle",	db_getAngle);
	funcTable->SetMember("getBool",		db_getBool);
	funcTable->SetMember("getInteger",	db_getInteger);

	funcTable->SetMember("toDouble", strToDouble);
	funcTable->SetMember("toInteger", strToInteger);
	funcTable->SetMember("toVector", strToVector);
	funcTable->SetMember("toAngle", strToAngle);
	funcTable->SetMember("toBool", strToBool);

	funcTable->SetMember("iter", new_iter);
	
	funcTable->SetMember("mhash", Murmur::MurmurHash);
	funcTable->SetMember("sha1", sha1::Sha1Hash);
	funcTable->SetMember("UID", UniqueID::getUniqueID);

	funcTable->SetMember("delete", database_delete);

	g_Lua->SetGlobal("leveldb", funcTable);
	g_Lua->SetGlobal("lvldb", funcTable);
	funcTable->UnReference();


	// adds s_ versions of methods. Basically makes them return value or nil on failure, no error returned.
	ILuaObject* fn_RunString = g_Lua->GetGlobal("RunString");
	g_Lua->Push(fn_RunString);
	g_Lua->Push(
			"local getInteger,setInteger=lvldb.getInteger,lvldb.setInteger;function lvldb.addInteger"
			"(a,b)local c,d=getInteger(a)if c then setInteger(a,d+b)else setInteger(a,b)end end;loca"
			"l getDouble,setDouble=lvldb.getDouble,lvldb.setDouble;function lvldb.addDouble(a,b)loca"
			"l c,d=getDouble(a)if c then setDouble(a,d+b)else setDouble(a,b)end end"
		);
	g_Lua->Call(1, 0);

	g_Lua->Push(fn_RunString);
	g_Lua->Push(
		"local function wrap(getter) return function(key)local succ, val = getter(key) if succ then return val end end end\n"
		"lvldb.s_getString = wrap(lvldb.getString)\n"
		"lvldb.s_getDouble = wrap(lvldb.getDouble)\n"
		"lvldb.s_getInteger = wrap(lvldb.getInteger)\n"
		"lvldb.s_getVector = wrap(lvldb.getVector)\n"
		"lvldb.s_getAngle = wrap(lvldb.getAngle)\n"
		"lvldb.s_getBool = wrap(lvldb.getBool)\n");
	g_Lua->Call(1, 0);

	fn_RunString->UnReference();

	return 0;
}

// Shutdown
int Shutdown(lua_State *L)
{
	return 0;
}
