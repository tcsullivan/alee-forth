CXXFLAGS += -std=c++17 -g3 -ggdb -O0 \
            -pedantic -Wall -Wextra -Werror -Weffc++ \
            -fno-exceptions -fno-threadsafe-statics -fno-rtti #-fstack-usage

CXXFILES := $(wildcard libalee/*.cpp)
OBJFILES := $(subst .cpp,.o,$(CXXFILES))
LIBFILE := libalee/libalee.a

all: alee

msp430: CXX := msp430-elf32-g++
msp430: AR := msp430-elf32-gcc-ar
msp430: CXXFLAGS += -Os -mmcu=msp430g2553 -ffunction-sections -fdata-sections
msp430: CXXFLAGS += -DMEMDICTSIZE=200 -flto
msp430: LDFLAGS += -L/opt/msp430-elf32/include -Tmsp430g2553.ld -Wl,-gc-sections
msp430: clean-lib alee-msp430

small: CXXFLAGS += -Os
small: alee

fast: CXXFLAGS += -O3 -march=native -mtune=native -flto
fast: alee

standalone: core.fth.h alee-standalone

alee: $(LIBFILE)
alee-msp430: $(LIBFILE)
alee-standalone: $(LIBFILE)

cppcheck:
	cppcheck --enable=warning,style,information --disable=missingInclude \
             libalee alee*.cpp *dict.hpp

test: standalone
	echo "\nbye\n" | ./alee-standalone test/tester.fr test/core.fr

$(LIBFILE): $(OBJFILES)
	$(AR) crs $@ $(OBJFILES)

core.fth.h: alee.dat
	xxd -i $< > $@
	sed -i "s/unsigned /static const &/" $@

alee.dat: alee core.fth
	echo "2 sys" | ./alee core.fth

clean: clean-lib
	rm -f alee alee-msp430 alee-standalone
	rm -f alee.dat core.fth.h

clean-lib:
	rm -f $(LIBFILE) $(OBJFILES)

.PHONY: all clean clean-lib cppcheck fast msp430 small standalone test

