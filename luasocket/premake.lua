dofile("../common.lua")

RequireDefaultlibs()
--RequireRuntime()


SOLUTION"luasocket"
	language"C"
	targetdir	"Release"
	INCLUDES	"lua51"
	includedirs	{"luasocket/src"}
	defines		{"NDEBUG"}
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		links		{"Ws2_32","lua_shared"}
		
	LINUX()

	PROJECT()
		--INCLUDES	"lua51"
		files		{"src/*.c"}
		configuration	("windows")
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
						"luasocket/src/udp.c",
						"luasocket/src/wsocket.c",
					}
		configuration	("linux")
			
			-- OH MY GOD PLEASE DONT LOOK
			linkoptions		{"-Wl,-rpath='$$ORIGIN'"}
			linkoptions		{"garrysmod/bin/lua_shared_srv.so"}
			prelinkcommands	{"mkdir -p garrysmod/bin && ln -s "..SRCDS_DIR.."/garrysmod/bin/lua_shared_srv.so garrysmod/bin/lua_shared_srv.so "}
			
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
						"luasocket/src/udp.c",
						"luasocket/src/unix.c",
						"luasocket/src/usocket.c",
					}