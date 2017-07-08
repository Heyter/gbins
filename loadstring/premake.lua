dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"loadstring"
	language"C"
	INCLUDES	"lua51"
	
	WINDOWS()
	LINUX()

	PROJECT()
		files	{"src/*.c"}
		INCLUDES	"lua51"
		
