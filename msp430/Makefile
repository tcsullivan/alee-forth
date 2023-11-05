dict: lzss
	cat msp430fr2476_symbols.ld | grep "^PROVIDE.*" | sed -e "s/^PROVIDE(//" -e "s/[ ][ ]*//" -e "s/=.0x/\\\\x/" -e "s/..);/\\\\x&/" -e "s/);//" > msp430fr2476_symbols.dat
	grep -E "^#define \w+\s+\([0-9].*$$" msp430fr2476.h | sed -e "s/).*$$/)/" -e "s/^#define //" -e "s/[ ][ ]*//" -e "s/(0x/\\\\x/" -e "s/..)/\\\\x&/" -e "s/)//" -e "s/(/\\\\x/" -e "s/\\\\x\\\\/\\\\x00\\\\/" > msp430fr2476.dat
	cat msp430fr2476_symbols.dat msp430fr2476.dat | tr '\n' '\373' | tr -d '\r' > msp430fr2476_all.dat
	@echo "printf ... > msp430fr2476_all.bin"
	@printf "$(shell cat msp430fr2476_all.dat)" > msp430fr2476_all.bin
	./lzss e msp430fr2476_all.bin msp430fr2476_all.lzss
	ls -l msp430fr2476_all.lzss
	xxd -i msp430fr2476_all.lzss > msp430fr2476_all.h

lzss: lzss.c

