CXXFLAGS += -std=c++17 -g3 -ggdb -O0 \
            -Wall -Wextra -pedantic -Werror \
	    -fno-exceptions -fno-rtti #-fstack-usage

CXXFILES := corewords.cpp dictionary.cpp parser.cpp state.cpp
OBJFILES := $(subst .cpp,.o,$(CXXFILES))
LIBFILE := libalee.a

all: alee

msp430: CXX := msp430-elf32-g++
msp430: AR := msp430-elf32-ar
msp430: CXXFLAGS += -Os -mmcu=msp430g2553 -ffunction-sections -fdata-sections -DMEMDICTSIZE=200
msp430: LDFLAGS += -L/opt/msp430-elf32/include -Wl,-gc-sections
msp430: alee-msp430

small: CXXFLAGS += -Os
small: alee

fast: CXXFLAGS += -O3 -march=native -mtune=native
fast: alee

alee: $(LIBFILE)

alee-msp430: $(LIBFILE)

$(LIBFILE): $(OBJFILES)
	$(AR) cr $@ $(OBJFILES)

clean:
	rm -f alee alee-msp430 $(LIBFILE) $(OBJFILES)

