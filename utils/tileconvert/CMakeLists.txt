# sources
SET(TILECONVERT_SRCS
	tileconvert.c
)

# targets
ADD_EXECUTABLE(tileconvert ${TILECONVERT_SRCS})
TARGET_LINK_LIBRARIES(tileconvert ${TTIP_LIBRARIES})
INSTALL(TARGETS tileconvert RUNTIME DESTINATION bin)
