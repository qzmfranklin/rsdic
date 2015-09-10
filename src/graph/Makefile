ROOT    :=$(realpath ../..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(ROOT)/src/rsdic/rules.mk
-include $(ROOT)/src/rx/rules.mk
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

check:
	@echo $(OBJ) $(TESTS)
	@echo $(D)/gtest_Tree.0

gtest_Tree.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_Tree.o $(D)/Tree.o \
	$(ROOT)/src/rsdic/RsdicBuilder.o $(ROOT)/src/rsdic/Rsdic.o $(ROOT)/src/rsdic/EnumCoder.o \
	$(ROOT)/src/rx/rx.o

gtest_Dag.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_Dag.o $(D)/Dag.o \
	$(ROOT)/src/rsdic/RsdicBuilder.o $(ROOT)/src/rsdic/Rsdic.o $(ROOT)/src/rsdic/EnumCoder.o \
	$(ROOT)/src/rx/rx.o
