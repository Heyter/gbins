dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luacurl"

	language"C"
	INCLUDES	"lua51"
	
	WINDOWS()
		
	LINUX()

	PROJECT()
		files	{"lua-curl/src/*.c"}
		files	{"src/*.c"}
		INCLUDES	"lua51"
		
		defines "DISABLE_INVALID_NUMBERS"
		
		configuration	("windows")
			includedirs	"curl-src/builds/libcurl-vc12-x86-release-dll-zlib-dll-ipv6-sspi-spnego-winssl/include"
			libdirs	"curl-src/builds/libcurl-vc12-x86-release-dll-zlib-dll-ipv6-sspi-spnego-winssl/lib"
			links	"libcurl"
		configuration	("linux")
			links	"curl"
			