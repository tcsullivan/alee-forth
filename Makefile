CXXFLAGS += -g3 -ggdb -O0

CXXFILES := corewords.cpp types.cpp
OBJFILES := $(subst .cpp,.o,$(CXXFILES))
EXEFILE := alee

all: alee

alee: $(OBJFILES)

clean:
	rm -f alee $(OBJFILES)

