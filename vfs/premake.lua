dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"vfs"
	
	INCLUDES	"source_sdk"
	INCLUDES	"backwards_headers"
	defines		{"NDEBUG","LUA_SERVER"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			includedirs 'X:/a/boost'
		configuration 		"linux"
			defines		{"USE_BOOST_THREADS"}
			removedefines	"PROTECTED_THINGS_ENABLE"
			links	{"pthread"}
			buildoptions { "-fpermissive" }
			