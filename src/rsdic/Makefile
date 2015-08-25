ROOT    :=$(realpath ../..)
d       :=$(ROOT)/src/rsdic# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(ROOT)/src/rsdic/rules.mk
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
	$(QUIET)echo "			======== CLEANUP ========"
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)echo "$(TESTS)"
	$(QUIET)echo "			======== TESTS ========"
	$(QUIET)for t in $^ ; do ./"$$t" ; done

# Sample linking rules for building executables:
#test_heapsort.exe: $(d)/heapsort.o $(d)/test_heapsort.o $(ROOT)/utils/utils.o
gtest_Rsdic.exe: $(d)/gtest_Rsdic.o $(d)/Rsdic.o $(ROOT)/gtest/gtest_main.a \
	$(d)/RsdicBuilder.o $(d)/EnumCoder.o
gtest_RsdicBuilder.exe: $(d)/gtest_RsdicBuilder.o $(d)/RsdicBuilder.o $(ROOT)/gtest/gtest_main.a \
	$(d)/Rsdic.o $(d)/EnumCoder.o
gtest_Util.exe: $(d)/gtest_Util.o $(ROOT)/gtest/gtest_main.a
gtest_EnumCoder.exe: $(d)/gtest_EnumCoder.o $(d)/EnumCoder.o $(ROOT)/gtest/gtest_main.a
gtest_BytesAligner.exe: $(d)/gtest_BytesAligner.o $(ROOT)/gtest/gtest_main.a
