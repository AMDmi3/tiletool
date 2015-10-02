# sources
SET(TILETOOL_SRCS
	bounds.c
	emptytile.c
	parsing.c
	paths.c
	process.c
	tiletool.c
)

# targets
ADD_EXECUTABLE(tiletool ${TILETOOL_SRCS})
TARGET_LINK_LIBRARIES(tiletool ${TTIP_LIBRARIES})
INSTALL(TARGETS tiletool RUNTIME DESTINATION bin)
