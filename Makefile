CXXFLAGS += -std=c++17 -g3 -ggdb -O0 \
            -Wall -Wextra -pedantic -Wno-vla -Werror

CXXFILES := corewords.cpp dictionary.cpp parser.cpp state.cpp types.cpp
OBJFILES := $(subst .cpp,.o,$(CXXFILES))
LIBFILE := libalee.a
EXEFILE := alee

all: $(EXEFILE)

small: CXXFLAGS += -Os
small: $(EXEFILE)

fast: CXXFLAGS += -O3 -march=native -mtune=native
fast: $(EXEFILE)

$(EXEFILE): $(LIBFILE)

$(LIBFILE): $(OBJFILES)
	$(AR) cr $@ $(OBJFILES)

clean:
	rm -f $(EXEFILE) $(LIBFILE) $(OBJFILES)

