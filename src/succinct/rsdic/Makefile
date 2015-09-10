ROOT    :=$(realpath ../../..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(D)/rules.mk
-include $(DEP)

# List all the executables you want to run when you type `make test` in $(TESTS)
# Note that you need to -include $(ROOT)/gtest/rules.mk to actually build those
# executables
TESTS:=gtest_Rsdic.exe gtest_RsdicBuilder.exe gtest_Util.exe gtest_EnumCoder.exe \
	gtest_BytesAligner.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" ; done

# Sample linking rules for building executables:
#test_heapsort.exe: $(D)/heapsort.o $(D)/test_heapsort.o $(ROOT)/utils/utils.o
gtest_Rsdic.exe: $(D)/gtest_Rsdic.o $(D)/Rsdic.o $(ROOT)/gtest/gtest_main.a \
	$(D)/RsdicBuilder.o $(D)/EnumCoder.o
gtest_RsdicBuilder.exe: $(D)/gtest_RsdicBuilder.o $(D)/RsdicBuilder.o $(ROOT)/gtest/gtest_main.a \
	$(D)/Rsdic.o $(D)/EnumCoder.o
gtest_Util.exe: $(D)/gtest_Util.o $(ROOT)/gtest/gtest_main.a
gtest_EnumCoder.exe: $(D)/gtest_EnumCoder.o $(D)/EnumCoder.o $(ROOT)/gtest/gtest_main.a
gtest_BytesAligner.exe: $(D)/gtest_BytesAligner.o $(ROOT)/gtest/gtest_main.a
