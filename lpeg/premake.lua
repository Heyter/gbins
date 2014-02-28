dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"lpeg"
	language"C"
	targetdir	"Release"
	INCLUDES	"lua51"
	includedirs	{"lpeg"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		
	LINUX()

	PROJECT()
		files	{"src/*.c"}
		files	{
					"lpeg/*.c",
				}
		
		INCLUDES	"lua51"
		
		configuration	("windows")
		configuration	("linux")