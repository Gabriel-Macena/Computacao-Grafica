#****************************************************************************

CC     := gcc
CXX    := g++
LD     := g++

LIBS		 :=
EXTRA_LIBS 	 := -lGL -lGLU -lglut

#****************************************************************************
# Targets of the build
#****************************************************************************

OUTPUT := trabalhocg

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************

SRCS := tinyxml.cpp tinyxmlparser.cpp trabalhocg.cpp tinyxmlerror.cpp tinystr.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS}
	${LD} -o $@ ${OBJS} ${LIBS} ${EXTRA_LIBS}

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -c $< -o $@

%.o : %.c
	${CC} -c $< -o $@

clean:
	-rm -f core ${OBJS} ${OUTPUT}


tinyxml.o: tinyxml.h tinystr.h
tinyxmlparser.o: tinyxml.h tinystr.h
trabalhocg.o: tinyxml.h tinystr.h
tinyxmlerror.o: tinyxml.h tinystr.h
