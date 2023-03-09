CXXFLAGS += -std=c++17 -g3 -ggdb -O0 \
            -Wall -Wextra -pedantic -Werror \
            -fno-exceptions -fno-rtti #-fstack-usage

CXXFILES := $(wildcard libalee/*.cpp)
OBJFILES := $(subst .cpp,.o,$(CXXFILES))
LIBFILE := libalee/libalee.a

all: alee

msp430: CXX := msp430-elf32-g++
msp430: AR := msp430-elf32-ar
msp430: CXXFLAGS += -Os -mmcu=msp430g2553 -ffunction-sections -fdata-sections
msp430: CXXFLAGS += -DMEMDICTSIZE=200
msp430: LDFLAGS += -L/opt/msp430-elf32/include -Wl,-gc-sections
msp430: clean-lib alee-msp430

small: CXXFLAGS += -Os
small: alee

fast: CXXFLAGS += -O3 -march=native -mtune=native -flto
fast: alee

standalone: core.fth.h alee-standalone

alee: $(LIBFILE)
alee-msp430: $(LIBFILE)
alee-standalone: $(LIBFILE)

$(LIBFILE): $(OBJFILES)
	$(AR) cr $@ $(OBJFILES)

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

.PHONY: all msp430 small fast standalone clean clean-lib

