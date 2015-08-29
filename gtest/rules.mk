# This is a hand crafted file to integrate the Google Test Framework into this
# Makefile system. This is significantly different from the rules.mk generated
# by the zmake script. Please do not change this file unless you really know
# what you are doing.
#
# Historically this file is very different from the main rules.mk files, but it
# is no longer that different. But still, keep it as it is for now.

TMP:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
$(TMP)DIR:=$(TMP)

$($(TMP)DIR)CC:=$(wildcard $($(TMP)DIR)/src/*.cc) $(wildcard $($(TMP)DIR)/samples/*.cc)

$($(TMP)DIR)CFLAGS:=$(CFLAGS)
$($(TMP)DIR)CXXFLAGS:=$(CXXFLAGS)
$($(TMP)DIR)INCS:=$(INCS)
$($(TMP)DIR)LIBS:=$(LIBS)

DEP+=${$($(TMP)DIR)CC:%.cc=%.d}
#OBJ+=${$($(TMP)DIR)CC:%.cc=%.o}
ASM+=${$($(TMP)DIR)CC:%.cc=%.s}

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.cc
	$(QUIET)$(CXX) -o $@ -c $< $(DEPFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.cc
	$(QUIET)$(CXX) -o $@ $< $(ASMFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

%.exe: $($(TMP)DIR)/samples/%.o
	$(QUIET)$(CXX) -o $@ $^ ${$($(TMP)DIR)LIBS}
	$(QUIET)echo "Linking $(MAGENTA)$(notdir $@) $(NONE)..."

$($(TMP)DIR)/gtest.a : $($(TMP)DIR)/src/gtest-all.o
	$(QUIET)$(AR) $(ARFLAGS) $@ $^
	$(QUIET)echo "Linking $(MAGENTA)$(notdir $@) $(NONE)..."

$($(TMP)DIR)/gtest_main.a : $($(TMP)DIR)/src/gtest-all.o $($(TMP)DIR)/src/gtest_main.o
	$(QUIET)$(AR) $(ARFLAGS) $@ $^
	$(QUIET)echo "Linking $(MAGENTA)$(notdir $@) $(NONE)..."
