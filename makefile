###############################################################################
#                                                                             #
# MAKEFILE for IP upload                                                      #
#                                                                             #
# (c) Guy Wilson 2021                                                         #
#                                                                             #
###############################################################################

# Version number for ipaddr
MAJOR_VERSION = 1
MINOR_VERSION = 1

# Directories
SOURCE = src
BUILD = build
DEP = dep

# What is our target
TARGET = ipaddr

# Tools
VBUILD = vbuild
CPP = g++
C = gcc
LINKER = g++

# postcompile step
PRECOMPILE = @ mkdir -p $(BUILD) $(DEP)
# postcompile step
POSTCOMPILE = @ mv -f $(DEP)/$*.Td $(DEP)/$*.d

CPPFLAGS = -c -O1 -Wall -pedantic -std=c++11
CFLAGS = -c -O1 -Wall -pedantic
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP)/$*.Td

# Libraries
STDLIBS = -pthread -lstdc++
EXTLIBS = -lcurl -lstrutils

COMPILE.cpp = $(CPP) $(CPPFLAGS) $(DEPFLAGS) $(MGFLAGS) -o $@
COMPILE.c = $(C) $(CFLAGS) $(DEPFLAGS) $(MGFLAGS) -o $@
LINK.o = $(LINKER) $(STDLIBS) -o $@

CSRCFILES = $(wildcard $(SOURCE)/*.c)
CPPSRCFILES = $(wildcard $(SOURCE)/*.cpp)
OBJFILES = $(patsubst $(SOURCE)/%.c, $(BUILD)/%.o, $(CSRCFILES)) $(patsubst $(SOURCE)/%.cpp, $(BUILD)/%.o, $(CPPSRCFILES))
DEPFILES = $(patsubst $(SOURCE)/%.c, $(DEP)/%.d, $(CSRCFILES)) $(patsubst $(SOURCE)/%.cpp, $(DEP)/%.d, $(CPPSRCFILES))

all: $(TARGET)

# Compile C/C++ source files
#
$(TARGET): $(OBJFILES)
	$(LINK.o) $^ $(EXTLIBS)

$(BUILD)/%.o: $(SOURCE)/%.c
$(BUILD)/%.o: $(SOURCE)/%.c $(DEP)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(BUILD)/%.o: $(SOURCE)/%.cpp
$(BUILD)/%.o: $(SOURCE)/%.cpp $(DEP)/%.d
	$(PRECOMPILE)
	$(COMPILE.cpp) $<
	$(POSTCOMPILE)

.PRECIOUS = $(DEP)/%.d
$(DEP)/%.d: ;

-include $(DEPFILES)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin
	cp ipaddr.cfg /usr/local/bin
	chmod 600 /usr/local/bin/ipaddr.cfg

version:
	$(VBUILD) -incfile ipaddr.ver -template version.c.template -out $(SOURCE)/version.c -major $(MAJOR_VERSION) -minor $(MINOR_VERSION)

clean:
	rm -r $(BUILD)
	rm -r $(DEP)
	rm $(TARGET)
