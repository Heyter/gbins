dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luaiox"
	language"C"
	targetdir	"Release"
	INCLUDES	"lua51"
	defines		{"NDEBUG"}
	
	WINDOWS()
		
	LINUX()

	PROJECT()
		--files	{"src/loslib.c"}
		files	{"src/lioxlib.c"}
		files	{"lfs/src/*.c"}
		files	{"src/main.c"}
		includedirs	{"lfs/src"}

		INCLUDES	"lua51"
		
		configuration	("windows")
		
		configuration	("linux")
		