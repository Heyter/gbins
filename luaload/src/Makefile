MODULE_NAME=gmsv_luasocket

# Path configuration
SOURCE_SDK=../../hl2sdk-ob-valve
SRCDS_DIR=../../../srcds/orangebox

PROJECT_DIR=.
OUT_DIR=../Release
OBJ_DIR=./Release

# Compilation Configuration
COMPILER=/usr/bin/g++-4.4
LINK=/usr/bin/g++-4.4

USER_CFLAGS=-m32
USER_LFLAGS=-m32

OPTFLAGS=-O1 -fomit-frame-pointer -ffast-math -fforce-addr -funroll-loops -fthread-jumps -fcrossjumping -foptimize-sibling-calls -fcse-follow-jumps -fcse-skip-blocks -fgcse -fgcse-lm -fexpensive-optimizations -frerun-cse-after-loop -fcaller-saves -fpeephole2 -fschedule-insns2 -fsched-interblock -fsched-spec -fregmove -fstrict-overflow -fdelete-null-pointer-checks -freorder-blocks -freorder-functions -falign-functions -falign-jumps -falign-loops -falign-labels -ftree-vrp -ftree-pre -finline-functions -funswitch-loops -fgcse-after-reload

LIBFILES= \
  lua_shared_srv.so \
  
MODULE_OBJS = \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/auxiliar.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/buffer.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/except.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/inet.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/io.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/luasocket.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/mime.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/options.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/select.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/tcp.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/timeout.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/udp.o \
	/home/srcds/developement/gm_luasocket/src/luasocket/src/usocket.o \
	$(OBJ_DIR)/main.o \

INCLUDE=-I$(PROJECT_DIR)/common -I$(PROJECT_DIR)/hde -I$(PROJECT_DIR)/sourcemod -I$(SOURCE_SDK)/public -I$(SOURCE_SDK)/common -I$(SOURCE_SDK)/public/steam -I$(SOURCE_SDK)/public/tier0 -I$(SOURCE_SDK)/public/tier1 -I$(SOURCE_SDK)/tier1 -I$(SOURCE_SDK)/public/game/server -I$(SOURCE_SDK)/game/shared -I$(SOURCE_SDK)/game/server -I$(SOURCE_SDK)/game/client

CFLAGS=$(USER_CFLAGS) $(OPTFLAGS) -mtune=i686 -march=pentium -mmmx -msse -pipe -D__LINUX__ -D_LINUX -Dsprintf_s=snprintf -Dstrcmpi=strcasecmp -D_alloca=alloca -DVPROF_LEVEL=1 -DSWDS -D_LINUX -DLINUX -DNDEBUG -fpermissive -Dstricmp=strcasecmp -D_stricmp=strcasecmp -Dstrcpy_s=strncpy -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp  -Usprintf=use_Q_snprintf_instead_of_sprintf -Ustrncpy=use_Q_strncpy_instead -Ufopen=dont_use_fopen -UPROTECTED_THINGS_ENABLE

LFLAGS=$(USER_LFLAGS) -lm -ldl $(LIBFILES) -shared -Wl

DO_CC=$(COMPILER) $(INCLUDE) -w $(CFLAGS) -DARCH=i486 -o $@ -c $<

all: dirs $(MODULE_NAME) install

dirs:
	-mkdir -p $(OUT_DIR)
	-mkdir -p $(OBJ_DIR)
	-mkdir -p $(OBJ_DIR)/sourcemod
	-mkdir -p $(OBJ_DIR)/tier0
	-mkdir -p $(OBJ_DIR)/common


$(MODULE_NAME): $(MODULE_OBJS) $(TIER0_OBJS) libs
	$(COMPILER) -o $(OUT_DIR)/$(MODULE_NAME)_linux.dll $(MODULE_OBJS) $(TIER1_OBJS) $(TIER0_OBJS) $(LFLAGS)

install:
	cp $(OUT_DIR)/$(MODULE_NAME)_linux.dll $(SRCDS_DIR)/garrysmod/lua/bin/ --remove-destination

libs:
	ln -f -s $(SRCDS_DIR)/bin/libtier0_srv.so
	ln -f -s $(SRCDS_DIR)/bin/libvstdlib_srv.so
	ln -f -s $(SRCDS_DIR)/bin/engine_srv.so
	ln -f -s $(SRCDS_DIR)/garrysmod/bin/lua_shared_srv.so
	ln -f -s $(SRCDS_DIR)/bin/libsteam_api.so

$(OBJ_DIR)/%.o : ./%.cpp
	$(DO_CC)

$(OBJ_DIR)/sourcemod/%.o: ./sourcemod/%.cpp
	$(DO_CC)

$(OBJ_DIR)/hde/%.o: ./hde/%.cpp
	$(DO_CC)

$(OBJ_DIR)/hde/hde32/src/%.o: ./hde/hde32/%.cpp
	$(DO_CC)
	
$(OBJ_DIR)/tier0/%.o: $(SOURCE_SDK)/public/tier0/%.cpp
	$(DO_CC)

$(OBJ_DIR)/common/%.o: ./common/%.cpp
	$(DO_CC)
 

clean:
	-rm -rf $(OBJ_DIR)
