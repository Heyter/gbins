#include "UniqueID.h"
#include "ILuaInterface.h"
#include "main.h"
#include "BaseEncode.h"

namespace UniqueID {
	int64_t curId;
	
	char* key = "__impl_uniqueid";
	size_t keyLen = 15;
	void updateDiskCounter() {
		char* err = NULL;
		db.put(key, keyLen, (char*)&curId, sizeof(int64_t), err);
		if (err!=NULL) 
			db.free(&err);
	}
	
	LUA_FUNCTION(getUniqueID) {
		ILuaInterface* g_Lua = Lua();

		char buff[sizeof(int64_t)*2]; // *2 since we're going down to base32
		
		size_t res = base32_encode((const char*)&curId, sizeof(int64_t), buff, sizeof(int64_t)* 2);
		curId++;

		g_Lua->Push(buff, res);

		if (curId % 0x10000 == 0)
			updateDiskCounter();

		return 1;
	}

	void initLua(ILuaInterface& g_Lua) {
		println(g_Lua, "UniqueID.cpp initializing");

		// first get the previous key block offset
		char* result = NULL;
		size_t length;
		bool success = db.get(key, keyLen, &result, length);

		if (success && length == sizeof(int64_t)) {
			int64_t* result_i = (int64_t*)result;
			curId = *result_i;
			curId += 0x10000;
		} else
			curId = 0;
			
		if (success && result!=NULL) {
			//db.free(&result);
		}
		
		updateDiskCounter();

		println(g_Lua, (" - key space block: " + to_string(curId)).c_str());
	}
}