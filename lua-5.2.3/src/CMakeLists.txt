project(LuaTest)

include_directories(AFTER ${CMAKE_SOURCE_DIR})

##########luaæ≤Ã¨ø‚
set(LIB_FILES lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)
source_group("\\libFiles" FILES ${LIB_FILES})
add_library (LuaLib STATIC ${LIB_FILES})



###########c++”ÎluaΩªª•###################
add_executable(LuaWithCPPTest source.cpp)
###########source_group("\\headFiles" FILES source.cpp)
target_link_libraries(LuaWithCPPTest LuaLib)


############luaΩ‚ Õ∆˜###########
add_executable(LuaInterpreter lua.c ${LIB_FILES})


#ADD_DEFINITIONS(-LUA_LIB -LUA_BUILD_AS_DLL)
######################################define LUA_LIB##################################################
######################################define LUA_BUILD_AS_DLL#########################################