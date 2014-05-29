local windows = os.get():find"windows" 
local mac = false
SOURCE_SDK = 
	windows and	[[source-sdk-2013-master/mp/src]]	-- windows
	or (mac	and	[[source-sdk-2013-master/mp/src]]) -- mac
			or	[[source-sdk-2013-master/mp/src]] -- linux

SRCDS_DIR = 
	windows	and	[[x:/g/srcds/steamapps/common/GarrysModDS]]
	or (mac	and	[[/path/to]])
			or	[[/home/srcds/srcds]]

STEAMWORKS_SDK = 
	windows	and	[[steamworks_sdk]] 
	or (mac	and	[[steamworks_sdk]])
			or	[[steamworks_sdk]]

