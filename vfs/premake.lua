dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"vfs"
	
	INCLUDES		"source_sdk"
	INCLUDES		"backwards_headers"
	
	
	WINDOWS()
	LINUX()

	PROJECT()
		targetprefix	"gmcl_"
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			includedirs 'X:/a/boost' -- HELP ME BROTHER
		configuration 		"linux"
			defines		{"USE_BOOST_THREADS"}
			removedefines	"PROTECTED_THINGS_ENABLE"
			links	{"pthread"}
			buildoptions { "-fpermissive" }
			