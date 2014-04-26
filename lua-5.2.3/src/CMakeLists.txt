project(LuaTest)

include_directories(AFTER ${CMAKE_SOURCE_DIR})

##########luaæ≤Ã¨ø‚
set(LIB_FILES lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)
source_group("\\libFiles" FILES ${LIB_FILES})
add_library (LuaLib SHARED ${LIB_FILES})



###########c++”ÎluaΩªª•###################
add_executable(LuaWithCPPTest source.cpp)
###########source_group("\\headFiles" FILES source.cpp)
target_link_libraries(LuaWithCPPTest LuaLib)


############luaΩ‚ Õ∆˜###########
add_executable(LuaInterpreter lua.c ${LIB_FILES})

############DLL#################
add_library(LuaDll SHARED sourceDll.cpp)
target_link_libraries(LuaDll LuaLib )


ADD_DEFINITIONS(-DLUA_LIB -DLUA_BUILD_AS_DLL)
######################################define LUA_LIB##################################################
######################################define LUA_BUILD_AS_DLL#########################################