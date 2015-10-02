PROJECT(tiletool C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# options
OPTION(WITH_TESTS "Build tests" ON)

SET(BENCHMARK_ITERATIONS 5000)

# depends & definitions
ADD_SUBDIRECTORY(libttip)

INCLUDE_DIRECTORIES(${TTIP_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

IF(NOT WIN32)
	ADD_DEFINITIONS(-DHAVE_FORK)
ENDIF(NOT WIN32)

ADD_DEFINITIONS(-std=c99)

# sources
SET(TILETOOL_SRCS
	tiletool/bounds.c
	tiletool/emptytile.c
	tiletool/parsing.c
	tiletool/process.c
	tiletool/tiletool.c
	tiletool/paths.c
)

SET(TILECONVERT_SRCS
	tileconvert/tileconvert.c
)

# subdirs
ADD_SUBDIRECTORY(tiletool)
ADD_SUBDIRECTORY(tileconvert)

IF(WITH_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF(WITH_TESTS)

# benchmark
MATH(EXPR BENCHMARK_LESS_ITERATIONS ${BENCHMARK_ITERATIONS}/10)

SET(BENCHMARK_BIN ${CMAKE_CURRENT_BINARY_DIR}/tileconvert/tileconvert)

ADD_CUSTOM_TARGET(benchmark
	COMMAND echo "Benchmarking image operations \\(${BENCHMARK_ITERATIONS} iterations\\)..."
	COMMAND ${BENCHMARK_BIN} -b ${BENCHMARK_ITERATIONS}
			loadpng ${PROJECT_SOURCE_DIR}/testdata/map00.png output_noop.png
	COMMAND ${BENCHMARK_BIN} -b ${BENCHMARK_LESS_ITERATIONS}
			savepng ${PROJECT_SOURCE_DIR}/testdata/map00.png output_noop.png
	COMMAND ${BENCHMARK_BIN} -b ${BENCHMARK_ITERATIONS}
			clone ${PROJECT_SOURCE_DIR}/testdata/map00.png output_clone.png
	COMMAND ${BENCHMARK_BIN} -b ${BENCHMARK_ITERATIONS}
			desaturate ${PROJECT_SOURCE_DIR}/testdata/map00.png output_desaturate.png
	COMMAND ${BENCHMARK_BIN} -b ${BENCHMARK_ITERATIONS}
			maskblend ${PROJECT_SOURCE_DIR}/testdata/map00.png ${PROJECT_SOURCE_DIR}/testdata/pt.png output_maskblend.png
	COMMAND ${BENCHMARK_BIN} -b ${BENCHMARK_ITERATIONS}
			downsample2x2
				${PROJECT_SOURCE_DIR}/testdata/map00.png ${PROJECT_SOURCE_DIR}/testdata/map10.png
				${PROJECT_SOURCE_DIR}/testdata/map01.png ${PROJECT_SOURCE_DIR}/testdata/map11.png
				output_downsample2x2.png
	COMMAND ${BENCHMARK_BIN} -b ${BENCHMARK_ITERATIONS}
			threshold 192 ${PROJECT_SOURCE_DIR}/testdata/map00.png output_threshold.png
	DEPENDS tileconvert
)
