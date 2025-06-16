include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_PROGRAMS = $(wildcard programs/*.c)
SOURCES_TESTS = $(wildcard tests/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM) $(SOURCES_PROGRAMS) $(SOURCES_TESTS)
	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(SOURCES) $(SOURCES_PROGRAMS) $(SOURCES_TESTS) -o ../$(MODULE)

	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 -no-pie _loader.c $(OBJECTS_ASM) $(SOURCES) $(SOURCES_PROGRAMS) $(SOURCES_TESTS) -o ../0000-sampleCodeModule.elf


obj/%.asm.o : asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf obj

.PHONY: all clean print