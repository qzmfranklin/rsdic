#  THIS DIRECTORY
TMP:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
$(TMP)DIR:=$(TMP)

#  ALL C/C++ FILES IN THIS DIRECTORY (WITHOUT PATHNAME)
$($(TMP)DIR)C  :=$(wildcard $(TMP)/*.c)
$($(TMP)DIR)CC :=$(wildcard $(TMP)/*.cc)
$($(TMP)DIR)CPP:=$(wildcard $(TMP)/*.cpp)
#  DIRECTORY-SPECIFIC COMPILING FLAGS AND INCLUDE DIRECTORIES
$($(TMP)DIR)CFLAGS:=$(CFLAGS)
$($(TMP)DIR)CXXFLAGS:=$(CXXFLAGS)
$($(TMP)DIR)INCS:=$(INCS)
$($(TMP)DIR)LIBS:=$(LIBS)

DEP:=$(DEP) $($($(TMP)DIR)C:%.c=%.d) $($($(TMP)DIR)CC:%.cc=%.d) $($($(TMP)DIR)CPP:%.cpp=%.d)
OBJ:=$(OBJ) $($($(TMP)DIR)C:%.c=%.o) $($($(TMP)DIR)CC:%.cc=%.o) $($($(TMP)DIR)CPP:%.cpp=%.o)
ASM:=$(ASM) $($($(TMP)DIR)C:%.c=%.s) $($($(TMP)DIR)CC:%.cc=%.s) $($($(TMP)DIR)CPP:%.cpp=%.s)

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.c
	$(QUIET)$(CC) -o $@ -c $< $(DEPFLAGS) $($($(TMP)DIR)CFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.c
	$(QUIET)$(CC) -o $@ $< $(ASMFLAGS) $($($(TMP)DIR)CFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.cc
	$(QUIET)$(CXX) -o $@ -c $< $(DEPFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.cc
	$(QUIET)$(CXX) -o $@ $< $(ASMFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.cpp
	$(QUIET)$(CXX) -o $@ -c $< $(DEPFLAGS) $($($(TMP)DIR)CXXFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.cpp
	$(QUIET)$(CXX) -o $@ $< $(ASMFLAGS) $($($(TMP)DIR)CXXFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

# Linking pattern rule for this directory
%.exe: $($(TMP)DIR)/%.o
	$(QUIET)$(CXX) -o $@ $^ $($($(TMP)DIR)LIBS)
	$(QUIET)echo "Linking $(MAGENTA)$(notdir $@) $(NONE)..."

# Recursive inclusion
-include $(wildcard $(TMP)/*/$(notdir $(lastword $(MAKEFILE_LIST))))
