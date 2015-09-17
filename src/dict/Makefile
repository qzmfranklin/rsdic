ROOT    :=$(realpath ../..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(D)/rules.mk
-include $(ROOT)/src/succinct/rules.mk
-include $(ROOT)/src/os/rules.mk
# Inlcude more rules.mk here if you this directory depends on them.
-include $(DEP)

TESTS:=gtest_WordTree.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" ; done

gtest_WordTree.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_WordTree.o $(D)/WordTree.o \
	$(ROOT)/src/os/path.o \
	$(ROOT)/src/succinct/rsdic/Rsdic.o \
	$(ROOT)/src/succinct/rsdic/BitVectorIfc.o \
	$(ROOT)/src/succinct/rsdic/EnumCoder.o \
	$(ROOT)/src/succinct/rx/rx.o \
