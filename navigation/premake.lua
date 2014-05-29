dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"navigation"
	
	INCLUDES	"source_sdk"
	INCLUDES	"backwards_headers"
	defines		{"NDEBUG"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
			defines		{"USE_BOOST_THREADS"}
			removedefines	"PROTECTED_THINGS_ENABLE"
			links	{"pthread","boost_thread"}
			buildoptions { "-fpermissive" }