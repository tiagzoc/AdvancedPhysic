cmake_minimum_required(VERSION 3.23)

project(TMM_Functional)

file(
	GLOB_RECURSE TMM_Functional_SRC
	CONFIGURE_DEPENDS
		src/*.cpp
		src/*.c
)    
source_group("src" FILES ${TMM_Functional_SRC})
message(STATUS "Library TMM_Functional_LIB Sources : ${TMM_Functional_SRC}")

file(
	GLOB_RECURSE TMM_Functional_HEADERS
	CONFIGURE_DEPENDS
		include/*.h
		include/*.hpp
		include/*.inl
)
source_group("include" FILES ${TMM_Functional_HEADERS})
message(STATUS "Library TMM_Functional_LIB Headers : ${TMM_Functional_HEADERS}")

add_library( TMM_Functional_LIB ${TMM_Functional_SRC} ${TMM_Functional_HEADERS} )  
target_compile_features( TMM_Functional_LIB PUBLIC cxx_std_20 )
target_include_directories( TMM_Functional_LIB PUBLIC include/ )

# REQUIRED
# - TMM_Setup
if(NOT TARGET TMM_Setup_LIB)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../TMM_Setup
		${CMAKE_CURRENT_BINARY_DIR}/TMM_Setup_build
	)
endif()

target_link_libraries(
	TMM_Functional_LIB
	PUBLIC
		TMM_Setup_LIB
)
