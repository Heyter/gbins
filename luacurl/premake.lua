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
		files	{"src/*.c"}
		files	{"lua-curl/src/*.c"}
		includedirs	"curl-src/builds/libcurl-vc12-x86-release-dll-zlib-dll-ipv6-sspi-spnego-winssl/include"
		libdirs	"curl-src/builds/libcurl-vc12-x86-release-dll-zlib-dll-ipv6-sspi-spnego-winssl/lib"
		links	"libcurl"
		INCLUDES	"lua51"
		
		configuration	("windows")
			defines "DISABLE_INVALID_NUMBERS"
		configuration	("linux")
		