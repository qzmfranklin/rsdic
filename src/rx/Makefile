ROOT    :=$(realpath ../..)
D	:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(D)/rules.mk
-include $(DEP)

# List all the executables you want to run when you type `make test` in $(TESTS)
# Note that you need to -include $(ROOT)/gtest/rules.mk to actually build those
# executables
TESTS:=rx_example.exe rx_test.exe gtest_rx.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" ; done

# Sample linking rules for building executables:
#test_heapsort.exe: $(D)/heapsort.o $(D)/test_heapsort.o $(ROOT)/utils/utils.o
#gtest_dag.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_dag.o $(D)/dag.o
rx_example.exe: $(D)/rx_example.o $(D)/rx.o
rx_test.exe: $(D)/rx_test.o $(D)/rx.o
gtest_rx.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_rx.o $(D)/rx.o
