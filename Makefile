CXXFLAGS += -std=c++17 -g3 -ggdb -O0

CXXFILES := corewords.cpp dictionary.cpp executor.cpp parser.cpp state.cpp \
	    types.cpp
OBJFILES := $(subst .cpp,.o,$(CXXFILES))
LIBFILE := libalee.a
EXEFILE := alee

all: $(EXEFILE)

small: CXXFLAGS += -Os
small: $(EXEFILE)

$(EXEFILE): $(LIBFILE)

$(LIBFILE): $(OBJFILES)
	$(AR) cr $@ $(OBJFILES)

clean:
	rm -f $(EXEFILE) $(LIBFILE) $(OBJFILES)

