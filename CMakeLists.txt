cmake_minimum_required(VERSION 3.28)
project(glPrey
	DESCRIPTION "An OpenGL enabled viewer for Prey (1995) BSP files"
	HOMEPAGE_URL "https://github.com/erysdren/glPrey"
	LANGUAGES C
	VERSION 1.0.0
)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(OpenGL REQUIRED)

find_library(LIBMATH m)

# executable

add_executable(glprey
	${CMAKE_CURRENT_SOURCE_DIR}/source/backend.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/glprey.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/wad.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/bsp.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/mip.c
)

if(TARGET SDL2::SDL2main)
	target_link_libraries(glprey PRIVATE SDL2::SDL2main)
endif()

if(LIBMATH)
	target_link_libraries(glprey PRIVATE ${LIBMATH})
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "TinyCC")
	target_compile_definitions(glprey PRIVATE SDL_DISABLE_IMMINTRIN_H)
endif()

target_link_libraries(glprey PRIVATE SDL2::SDL2 OpenGL::GL OpenGL::GLU)

# additional tools

add_executable(bsp2ply
	${CMAKE_CURRENT_SOURCE_DIR}/source/bsp.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/bsp2ply.c
)

add_executable(wad2png
	${CMAKE_CURRENT_SOURCE_DIR}/source/mip.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/wad.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/wad2png.c
)

if(LIBMATH)
	target_link_libraries(wad2png PRIVATE ${LIBMATH})
endif()
