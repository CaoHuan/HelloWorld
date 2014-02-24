project(Lua)
add_library (lua SHARED lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)
 ADD_DEFINITIONS(-LUA_LIB -LUA_BUILD_AS_DLL)

######################################define LUA_LIB##################################################
######################################define LUA_BUILD_AS_DLL#########################################