CXXFLAGS += -std=c++20 -g3 -ggdb -O0 \
            -pedantic -Wall -Wextra -Werror -Weffc++ -Wconversion \
            -fno-exceptions -fno-rtti

CXXFILES := $(wildcard libalee/*.cpp)
OBJFILES := $(subst .cpp,.o,$(CXXFILES))
LIBFILE := libalee/libalee.a

STANDALONE := forth/core.fth

all: alee

msp430: CXX := msp430-elf-g++
msp430: AR := msp430-elf-gcc-ar
msp430: CXXFLAGS += -I. -I/usr/msp430-elf/usr/include
msp430: CXXFLAGS += -Os -mmcu=msp430fr2476 -ffunction-sections -fdata-sections
msp430: CXXFLAGS += -flto -fno-asynchronous-unwind-tables -fno-threadsafe-statics -fno-stack-protector
msp430: LDFLAGS += -L msp430 -T msp430fr2476.ld -Wl,-gc-sections -Wl,--no-warn-rwx-segments
msp430: msp430/alee-msp430

msp430-prep: CXXFLAGS += -DALEE_MSP430 -Imsp430
msp430-prep: msp430/msp430fr2476_all.h
msp430-prep: STANDALONE += forth/core-ext.fth forth/tools.fth forth/msp430.fth
msp430-prep: core.fth.h
msp430-prep: clean-lib

small: CXXFLAGS += -Os -fno-asynchronous-unwind-tables -fno-threadsafe-statics -fno-stack-protector
small: alee

fast: CXXFLAGS += -O3 -march=native -mtune=native -flto
fast: alee

standalone: core.fth.h
standalone: alee-standalone

alee: $(LIBFILE)
msp430/alee-msp430: $(LIBFILE)
alee-standalone: $(LIBFILE)

cppcheck:
	cppcheck --enable=warning,style,information --disable=missingInclude \
             libalee alee*.cpp *dict.hpp

test: standalone
	echo "bye" | ./alee-standalone forth/core-ext.fth tests/src/tester.fr tests/src/core.fr

$(LIBFILE): $(OBJFILES)
	$(AR) crs $@ $(OBJFILES)

core.fth.h: alee.dat
	xxd -i $< > $@
	sed -i "s/\[\]/\[ALEE_RODICTSIZE\]/" $@

alee.dat: alee $(STANDALONE)
	echo "3 sys" | ./alee $(STANDALONE)

msp430/msp430fr2476_all.h:
	$(MAKE) -C msp430

clean: clean-lib
	rm -f alee alee-standalone msp430/alee-msp430
	rm -f alee.dat core.fth.h

clean-lib:
	rm -f $(LIBFILE) $(OBJFILES)

.PHONY: all clean clean-lib cppcheck fast msp430 small standalone test

