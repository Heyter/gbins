dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luafilesystem"
	language"C"
	targetdir	"Release"
	INCLUDES	"lua51"
	includedirs	{"luafilesystem/src"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		
	LINUX()

	PROJECT()
		files	{"src/*.c"}
		files	{LUA51.."/src/liolib.c"}
		--files	{"luafilesystem/src/lfs.c"}

		INCLUDES	"lua51"
		
		configuration	("windows")
		
		configuration	("linux")
		