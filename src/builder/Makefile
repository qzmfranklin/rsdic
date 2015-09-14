ROOT    :=$(realpath ../..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(ROOT)/src/rules.mk
-include $(D)/rules.mk
# Inlcude more rules.mk here if you this directory depends on them.
-include $(DEP)

TESTS:=gtest_builder.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" ; done

gtest_builder.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_builder.o \
	$(ROOT)/src/builder/tree/Tree.o \
	$(ROOT)/src/succinct/rsdic/RsdicBuilder.o $(ROOT)/src/succinct/rsdic/Rsdic.o \
	$(ROOT)/src/succinct/rsdic/EnumCoder.o $(ROOT)/src/succinct/rx/rx.o \
	$(ROOT)/src/os/path.o $(ROOT)/src/os/os.o
