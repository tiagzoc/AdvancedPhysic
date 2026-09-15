cmake_minimum_required(VERSION 3.23)

project(TMM_Setup)

file(
	GLOB_RECURSE TMM_Setup_SRC
	CONFIGURE_DEPENDS
		src/*.cpp
		src/*.c
)    
source_group("src" FILES ${TMM_Setup_SRC})
message(STATUS "Library TMM_Setup_LIB Sources : ${TMM_Setup_SRC}")

file(
	GLOB_RECURSE TMM_Setup_HEADERS
	CONFIGURE_DEPENDS
		include/*.h
		include/*.hpp
		include/*.inl
)
source_group("include" FILES ${TMM_Setup_HEADERS})
message(STATUS "Library TMM_Setup_LIB Headers : ${TMM_Setup_HEADERS}")

add_library( TMM_Setup_LIB ${TMM_Setup_SRC} ${TMM_Setup_HEADERS} )    
target_compile_features( TMM_Setup_LIB PUBLIC cxx_std_20 )
target_include_directories( TMM_Setup_LIB PUBLIC include/ )

