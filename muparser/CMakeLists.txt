
# Create a library from muParser project 

set (SRC_FILES
	./src/muParser.cpp
	./src/muParserBase.cpp
	./src/muParserBytecode.cpp
	./src/muParserCallback.cpp
	./src/muParserError.cpp
	./src/muParserTokenReader.cpp
)

set (CMAKE_BINARY_DIR build)

include_directories (include)
add_library (muparser ${SRC_FILES})
