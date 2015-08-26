ROOT    :=$(realpath ../..)
d       :=$(ROOT)/src/rx# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
#-include $(ROOT)/src/rsdic/rules.mk
-include $(ROOT)/src/rx/rules.mk
-include $(DEP)

# List all the executables you want to run when you type `make test` in $(TESTS)
# Note that you need to -include $(ROOT)/gtest/rules.mk to actually build those
# executables
TESTS:=rx_example.exe rx_test.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)echo "			======== CLEANUP ========"
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)echo "			======== TESTS ========"
	$(QUIET)for t in $^ ; do ./"$$t" ; done

# Sample linking rules for building executables:
#test_heapsort.exe: $(d)/heapsort.o $(d)/test_heapsort.o $(ROOT)/utils/utils.o
#gtest_dag.exe: $(ROOT)/gtest/gtest_main.a $(d)/gtest_dag.o $(d)/dag.o
rx_example.exe: $(d)/rx_example.o $(d)/rx.o
rx_test.exe: $(d)/rx_test.o $(d)/rx.o
