cmake_minimum_required(VERSION 3.23)

project(PhysicAdvanced LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    include/*.h
    include/*.hpp
    include/*.inl
)
source_group("include" FILES ${HEADERS})

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    src/*.c
    src/*.cpp
)
source_group("src" FILES ${SRC})

include(FetchContent)

# ---- GLFW ----
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(glfw)

# ---- OpenGL ----
find_package(OpenGL REQUIRED)

# ---- STB ----
include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master  # Use a specific tag like v2.14 for stability
)

FetchContent_MakeAvailable(stb)


# ---- glm ----
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(glm)

add_executable(${PROJECT_NAME} ${SRC} ${HEADERS})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE include)

add_subdirectory("TMM_Functional/")
add_subdirectory("TMM_Threading/")

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        OpenGL::GL
        glfw
        glm
	    TMM_Functional_LIB
        TMM_Threading_LIB
)
# Now you can use stb headers in your project
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})   

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/res"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
            "POST BUILD EXECUTED FOR ${PROJECT_NAME} : Copy from : ${CMAKE_CURRENT_SOURCE_DIR}/res to $<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Optimisations spécifiques Release
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)