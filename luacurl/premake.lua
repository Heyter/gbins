dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luacurl"
	language"C"
	targetdir	"Release"
	INCLUDES	"lua51"
	defines		{"NDEBUG"}
	
	WINDOWS()
		
	LINUX()

	PROJECT()
		files	{"lua-curl/src/*.c"}
		files	{"src/*.c"}
		INCLUDES	"lua51"
		
		configuration	("windows")
			includedirs	"curl-src/builds/libcurl-vc12-x86-release-dll-zlib-dll-ipv6-sspi-spnego-winssl/include"
			libdirs	"curl-src/builds/libcurl-vc12-x86-release-dll-zlib-dll-ipv6-sspi-spnego-winssl/lib"
			links	"libcurl"
			defines "DISABLE_INVALID_NUMBERS"
		configuration	("linux")
			links	"curl"
			