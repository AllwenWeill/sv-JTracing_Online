SVJ_PATH = ../sv-JTracing
Q = @
COMPILE = /usr/bin/
CC = ${Q}${COMPILE}gcc-9
CXX = ${Q}${COMPILE}g++-9
SIZE = ${Q}${COMPILE}size
CFLAGS = -std=c++17 -Wall -g 
ECHO = ${Q}echo
TARGET = server

INCLUDE_PATH = -I${SVJ_PATH}/include/preprocessing/ \
-I${SVJ_PATH}/include/parsing/ \
-I${SVJ_PATH}/include/utility/ \
-I${SVJ_PATH}/include/syntax/ \
-I${SVJ_PATH}/include/ast/ \
-I${SVJ_PATH}/include/handlMessage/ \

SRC_PATH = ../code/http/httpconn.cpp \
../code/http/httprequest.cpp \
../code/http/httpresponse.cpp \
../code/server/epoller.cpp \
../code/server/webserver.cpp \
../code/buffer/buffer.cpp \
../code/main.cpp \
${SVJ_PATH}/source/preprocessing/SourceManager.cpp \
${SVJ_PATH}/source/parsing/Lexer.cpp \
${SVJ_PATH}/source/parsing/Token.cpp \
${SVJ_PATH}/source/parsing/TokenKind.cpp \
${SVJ_PATH}/source/parsing/Parser.cpp \
${SVJ_PATH}/source/handlMessage/LogError.cpp \
${SVJ_PATH}/source/handlMessage/LogParser.cpp

OFILE = $(patsubst %.cpp,%.o,$(SRC_PATH))

LDFLAGS = ${INCLUDE_PATH}
CXXFLAGS = ${INCLUDE_PATH} ${CFLAGS}

all: ${OFILE}
	${ECHO} "cxx all $@"
	$(CXX) ${LDFLAGS} ${OFILE} -o ../bin/$(TARGET) -pthread
	${SIZE} ../bin/$(TARGET)

${OFILE}: ${SRC_PATH}
	${ECHO} "cxx $@"
	${CXX} ${CXXFLAGS} -c $(patsubst %.o,%.cpp,$@) -o $@ 

.PHONY : clean
clean:
	rm -rf ../bin/* $(TARGET)
	rm ${OFILE}




