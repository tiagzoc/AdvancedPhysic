cmake_minimum_required(VERSION 3.23)

project(TMM_Threading)

file(
	GLOB_RECURSE TMM_Threading_SRC
	CONFIGURE_DEPENDS
		src/*.cpp
		src/*.c
)  
source_group("src" FILES ${TMM_Threading_SRC})  
message(STATUS "Library TMM_Threading_LIB Sources : ${TMM_Threading_SRC}")

file(
	GLOB_RECURSE TMM_Threading_HEADERS
	CONFIGURE_DEPENDS
		include/*.h
		include/*.hpp
		include/*.inl
)
source_group("include" FILES ${TMM_Threading_HEADERS})
message(STATUS "Library TMM_Threading_LIB Headers : ${TMM_Threading_HEADERS}")

add_library( TMM_Threading_LIB ${TMM_Threading_SRC} ${TMM_Threading_HEADERS} )   
target_compile_features( TMM_Threading_LIB PUBLIC cxx_std_20 )
target_include_directories( TMM_Threading_LIB PUBLIC include/ )

# REQUIRED
# - TMM_Setup
# - TMM_Functional
if(NOT TARGET TMM_Setup_LIB)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../TMM_Setup
		${CMAKE_CURRENT_BINARY_DIR}/TMM_Setup_build
	)
endif()
if(NOT TARGET TMM_Functional_LIB)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../TMM_Functional
		${CMAKE_CURRENT_BINARY_DIR}/TMM_Functional_build
	)
endif()

target_link_libraries(
	TMM_Threading_LIB
	PUBLIC
		TMM_Setup_LIB
		TMM_Functional_LIB
)
