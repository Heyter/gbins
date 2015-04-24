dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luaload"
	language"C"
	INCLUDES	"lua51"
	
	WINDOWS()
	LINUX()

	PROJECT()
		files	{"src/*.c"}
		INCLUDES	"lua51"
		
