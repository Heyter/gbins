DEBUG=true
dofile("../common.lua")

-- for some reason it wont compile otherwise
--RequireDefaultlibs()
--RequireRuntime()

SOLUTION"leveldb"
	
	INCLUDES	"source_sdk"
	INCLUDES	"backwards_headers"
	
	WINDOWS()
	
	LINUX()
		libdirs	"leveldb"
		links{"leveldb/leveldb"}
		--links_static"leveldb"
		links{"pthread"}

	PROJECT()

		includedirs "leveldb/include"

		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
	                buildoptions { "-std=c++1y" }
