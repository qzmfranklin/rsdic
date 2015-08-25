#  THIS DIRECTORY
DIRf8087e59fd95af1ae29e8fcb7ff1a3dc:=$(ROOT)/src/rsdic
#  ALL C/C++ FILES IN THIS DIRECTORY (WITHOUT PATHNAME)
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)C:=$(wildcard *.c)
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CPP:=$(wildcard *.cpp)
#  DIRECTORY-SPECIFIC COMPILING FLAGS AND INCLUDE DIRECTORIES
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CFLAGS:=$(CFLAGS)
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CXXFLAGS:=$(CXXFLAGS)
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)INCS:=$(INCS)
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)LIBS:=$(LIBS)

DEP+=$($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CPP:%.cpp=$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.d) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)C:%.c=$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.d)
OBJ+=$($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CPP:%.cpp=$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.o) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)C:%.c=$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.o)
ASM+=$($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CPP:%.cpp=$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.s) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)C:%.c=$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.s)

$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.o: $(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.c
	$(QUIET)$(CC) -o $@ -c $< $(DEPFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)INCS)
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.s: $(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.c
	$(QUIET)$(CC) -o $@ $< $(ASMFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)INCS)
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.o: $(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.cpp
	$(QUIET)$(CXX) -o $@ -c $< $(DEPFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CXXFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)INCS)
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.s: $(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.cpp
	$(QUIET)$(CXX) -o $@ $< $(ASMFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)CXXFLAGS) $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)INCS)
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

# Linking pattern rule for this directory
%.exe: $(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)/%.o
	$(QUIET)$(CXX) -o $@ $^ $($(DIRf8087e59fd95af1ae29e8fcb7ff1a3dc)LIBS)
	$(QUIET)echo "Linking $(MAGENTA)$(notdir $@) $(NONE)..."
