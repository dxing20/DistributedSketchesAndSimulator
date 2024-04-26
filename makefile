# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Makefile settings - Can change according to your project needs.
TARGET = main   # Name of the executable.
SRCDIR = .
OBJDIR = build
BINDIR = bin

SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
INCLUDES := $(shell find $(SRCDIR) -name '*.h' -or -name '*.hpp')
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

$(BINDIR)/$(TARGET): $(OBJECTS)
	@echo "Creating bin directory"
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ -lz -lpcap

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(BINDIR)

# Dependency rules
-include $(OBJECTS:.o=.d)

# Dependency rule template
$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< -MF $@