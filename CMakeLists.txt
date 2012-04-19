PROJECT(tiletool)

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

# targets
ADD_EXECUTABLE(tiletool ${TILETOOL_SRCS})
TARGET_LINK_LIBRARIES(tiletool ${TTIP_LIBRARIES})
INSTALL(TARGETS tiletool RUNTIME DESTINATION bin)

ADD_EXECUTABLE(tileconvert ${TILECONVERT_SRCS})
TARGET_LINK_LIBRARIES(tileconvert ${TTIP_LIBRARIES})
INSTALL(TARGETS tileconvert RUNTIME DESTINATION bin)

# tests
IF(WITH_TESTS)
	ENABLE_TESTING()

	# libttip tests
	ADD_EXECUTABLE(createdestroy_test tests/createdestroy.c)
	TARGET_LINK_LIBRARIES(createdestroy_test ${TTIP_LIBRARIES})
	ADD_TEST(createdestroy createdestroy_test)

	ADD_EXECUTABLE(pngio_test tests/pngio.c)
	TARGET_LINK_LIBRARIES(pngio_test ${TTIP_LIBRARIES})
	ADD_TEST(pngio pngio_test)

	ADD_EXECUTABLE(errors_test tests/errors.c)
	TARGET_LINK_LIBRARIES(errors_test ${TTIP_LIBRARIES})
	ADD_TEST(errors errors_test)

	# tiletool internals tests
	INCLUDE_DIRECTORIES(tiletool)

	ADD_EXECUTABLE(parsing_test tests/parsing.c tiletool/parsing.c)
	ADD_TEST(parsing parsing_test)

	ADD_EXECUTABLE(bounds_test tests/bounds.c tiletool/bounds.c tiletool/parsing.c)
	ADD_TEST(bounds bounds_test)

	ADD_EXECUTABLE(paths_test tests/paths.c tiletool/paths.c)
	ADD_TEST(paths paths_test)

	ADD_EXECUTABLE(process_test tests/process.c tiletool/process.c)
	ADD_TEST(process process_test)
ENDIF(WITH_TESTS)

# benchmark
MATH(EXPR BENCHMARK_LESS_ITERATIONS ${BENCHMARK_ITERATIONS}/10)

SET(BENCHMARK_BIN ${CMAKE_CURRENT_BINARY_DIR}/tileconvert)

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
