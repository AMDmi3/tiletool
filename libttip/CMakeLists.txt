PROJECT(libttip C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# options
OPTION(WITH_PNG "Include PNG support" ON)
OPTION(WITH_VERBOSE "Print verbose warning messages to stderr" ON)
OPTION(WITH_TESTS "Build tests" ON)

# depends & definitions
INCLUDE_DIRECTORIES(.)

IF(WITH_PNG)
	FIND_PACKAGE(PNG REQUIRED)
	ADD_DEFINITIONS(-DWITH_PNG)

	INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIRS})
ENDIF(WITH_PNG)

IF(WITH_VERBOSE)
	ADD_DEFINITIONS(-DWITH_VERBOSE)
ENDIF(WITH_VERBOSE)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# sources
SET(TTIP_SRCS
	basic.c
	core.c
	pixel.c
	png.c
	tr_desaturate.c
	tr_downsample2x2.c
	tr_maskblend.c
	tr_threshold.c
)

# library
ADD_LIBRARY(ttip STATIC ${TTIP_SRCS})

# set parent scope variables so library can be bundled in other projects
SET(TTIP_LIBRARIES ttip ${PNG_LIBRARIES} PARENT_SCOPE)
SET(TTIP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
