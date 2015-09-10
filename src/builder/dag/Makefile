ROOT    :=$(realpath ../../..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(ROOT)/src/os/rules.mk
-include $(ROOT)/src/succinct/rules.mk
-include $(D)/rules.mk
# Inlcude more rules.mk here if you this directory depends on them.
-include $(DEP)

TESTS:=gtest_Tree.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" ; done

gtest_Dag.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_Dag.o $(D)/Dag.o \
	$(ROOT)/src/succinct/rsdic/RsdicBuilder.o $(ROOT)/src/succinct/rsdic/Rsdic.o \
	$(ROOT)/src/succinct/rsdic/EnumCoder.o $(ROOT)/src/succinct/rx/rx.o
