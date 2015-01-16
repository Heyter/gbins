dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luasocket"
	language"C"
	INCLUDES	"lua51"
	includedirs	{"luasocket/src"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		links		{"Ws2_32"}
		
	LINUX()

	PROJECT()
		files	{"src/*.c"}
		files	{
					"luasocket/src/auxiliar.c",
					"luasocket/src/buffer.c",
					"luasocket/src/except.c",
					"luasocket/src/inet.c",
					"luasocket/src/io.c",
					"luasocket/src/luasocket.c",
					"luasocket/src/mime.c",
					"luasocket/src/options.c",
					"luasocket/src/select.c",
					"luasocket/src/tcp.c",
					"luasocket/src/timeout.c",
					"luasocket/src/udp.c"
				}
		
		INCLUDES	"lua51"
		
		configuration	("windows")
			files		{"luasocket/src/wsocket.c"}
		configuration	("linux")			
			files		{	
							"luasocket/src/unix.c",
							"luasocket/src/usocket.c"
						}