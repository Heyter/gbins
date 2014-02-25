dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luaio"
	language"C"
	targetdir	"Release"
	INCLUDES	"lua51"
	defines		{"NDEBUG"}
	
	WINDOWS()
		
	LINUX()

	PROJECT()
		files	{"src/*.c"}
		files	{LUA51.."/src/liolib.c"}

		INCLUDES	"lua51"
		
		configuration	("windows")
		
		configuration	("linux")
		