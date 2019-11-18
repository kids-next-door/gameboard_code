# 
# 2019-04-28 Jim Fischer <fischerjd@missouri.edu>
#

# The name of the program to be created.
PROGRAM := demo

# The directory wherein the auto-generated dependency files will reside.
DEPDIR := .d

# Automatic creation of dependency rules (via the C preprocessor).
# -MT target
#  		Specifies a custom target for the makefile rule.
# -MMD
#  		List only user header files, not the system headers, in the makefile
#  		rule's dependency list. (See also: -MD)
# -MF file
#       Specifies the output file to write the makefile dependency rule into.
#
DEPFLAGS = -MT $@ -MMD -MF $(DEPDIR)/$*.Td

# C compiler flags
CFLAGS.common := -std=c11
CFLAGS.optimize := -O3
CFLAGS := $(CFLAGS.common) $(CFLAGS.optimize)
COMPILE.c = $(CC) $(DEPFLAGS) $(CPPFLAGS) $(CFLAGS) $(TARGET_ARCH) -c

# C++ compiler flags
CXXFLAGS.common := -std=c++11
CXXFLAGS.optimize := -O3
CXXFLAGS := $(CXXFLAGS.common) $(CXXFLAGS.optimize)
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) -c

# Post-compile tasks
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

# Use make's `wildcard' function to find all of the C source files (.c) in the
# current working directory. Next, use a substitution reference to generate the
# list of C object files (.o) from the list of C source code files. 
# (See also make's `patsubst' function.)
CSOURCES := $(wildcard *.c)
COBJS := $(CSOURCES:.c=.o)

# Use make's `wildcard' function to find all of the C++ source files (.cc,.cpp)
# in the in the current working directory. Next, use a substitution reference
# to generate the list of C++ object files (.o) from the list of C++ source
# files.  (See also make's `patsubst' function.)
CCSOURCES := $(wildcard *.cc)
CCOBJS := $(CCSOURCES:.cc=.o)
CPPSOURCES := $(wildcard *.cpp)
CPPOBJS += $(CPPSOURCES:.cpp=.o)

# Aggregate lists of C/C++ sources and their object files.
SOURCES := $(CSOURCES) $(CCSOURCES) $(CPPSOURCES)
OBJS    := $(COBJS) $(CCOBJS) $(CPPOBJS)

# Use make's wildcard function to find all of the header files in the current
# working directory.
HEADERS := $(wildcard *.h) $(wildcard *.hh) $(wildcard *.hpp) $(wildcard *.H)

# [Optional]  If you are building software for the Raspberry Pi, store a copy
# of the makefile `rpi.mk' in the current working directory. The `rpi.mk'
# makefile provides a) cross compiling support when building your RPI3B
# programs on a desktop computer, and b) preprocessor, compiler, and linker
# options that allow your programs to use the wiringPi library.
ifneq ($(wildcard rpi.mk),)
include rpi.mk
endif

# [Optional]  Additional customizations for this build should be defined in a
# makefile named `customize.mk' in the current working directory.
ifneq ($(wildcard custom.mk),)
include custom.mk
endif

# [Optional]  If support for multi-threaded execution is needed, define those
# options in a makefile named `threads.mk' in the current working directory.
ifneq ($(wildcard threads.mk),)
include threads.mk
endif


# The default build target: `all'
.PHONY : all
all : do_all

# Builds a 'debug' version of the program.
#
# NOTE: As stated in the gcc manual, "If you use multiple -O options, with or
# without level numbers, the last such option is the one that is effective."
#
# See also "6.11 Target-specific Variable Values" in the GNU Make manual.
.PHONY : debug
debug : CPPFLAGS += -DTRACE_LEVEL=1
debug : CFLAGS := $(CFLAGS.common) -Og -ggdb
debug : CXXFLAGS := $(CXXFLAGS.common) -Og -ggdb
debug : do_all

$(PROGRAM) : $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Cancel make's implicit rule for .c -> .o translation and replace it with the
# one below.
%.o : %.c

# See reference [2] for $(OUTPUT_OPTION) == `-o $@'
# See reference [4] "Make ignoring Prerequisite that doesn't exist"
# See also "order-only prerequisites" in the GNU Make manual.
$(COBJS) : %.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

# Cancel make's implicit rule for .cc -> .o translation and replace it with the
# one below.
%.o : %.cc

# See reference [2] for $(OUTPUT_OPTION) == `-o $@'
# See reference [4] "Make ignoring Prerequisite that doesn't exist"
# See also "order-only prerequisites" in the GNU Make manual.
$(CCOBJS) : %.o : %.cc $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

# Cancel make's implicit rule for .cpp -> .o translation and replace it with
# the one below.
%.o : %.cpp

# See reference [2] for $(OUTPUT_OPTION) == `-o $@'
# See reference [4] "Make ignoring Prerequisite that doesn't exist"
# See also "order-only prerequisites" in the GNU Make manual.
$(CPPOBJS) : %.o : %.cpp $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

# Creates the dependency directory if it doesn't exist.
$(DEPDIR) : ; mkdir -p $@ >/dev/null

# See the section "Avoiding 'No rule to make target ...' Errors" in [1].
$(DEPDIR)/%.d: ;

# "Mark the dependency files precious to make, so they won't be automatically
# deleted as intermediate files" [1].
.PRECIOUS: $(DEPDIR)/%.d

# Include the automatically-generated dependency rules from $(DEPDIR)
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES))))

# Housekeeping
.PHONY : clean
clean :
	rm -f $(OBJS) $(PROGRAM)
	rm -fr $(DEPDIR)

# Print the values of the environment variables and makefile variables (useful
# for debugging the makefile).
# Example:  $ make vars | less
.PHONY : vars
vars :
	$(foreach V, $(sort $(.VARIABLES)), $(if $(filter-out default automatic, $(origin $V)), $(info $V=$($V) ($(value $V)))))

# [Optional] If you want to perform additional tasks after the PROGRAM is
# built, create in the current working directory a makefile named `check.mk'.
# In that makefile define a .PHONY rule named `check' whose first prerequisite
# is `$(PROGRAM)' followed by other prerequisites that should be satisfied
# after the $(PROGRAM) program prerequisite is satisfied, e.g.,
#
# 		.PHONY : check
# 		check : $(PROGRAM) ...
#
CHECK_MAKEFILE := ./check.mk
ifneq ($(wildcard $(CHECK_MAKEFILE)),)
include $(CHECK_MAKEFILE)
endif

# [Optional] If you want to perform additional tasks after the PROGRAM is
# built, create in the current working directory a makefile named
# `postbuild.mk'.  In that makefile define a make variable `POSTBUILD' whose
# value is `postbuild'. Then define a .PHONY rule named `postbuild' whose first
# prerequisite is `$(PROGRAM)' followed by other prerequisites that should
# be satisfied after the $(PROGRAM) program prerequisite is satisfied, e.g.,
#
# 		POSTBUILD := postbuild
# 		.PHONY : postbuild
# 		postbuild : $(PROGRAM) ...
#
POSTBUILD_MAKEFILE := ./postbuild.mk
ifneq ($(wildcard $(POSTBUILD_MAKEFILE)),)
include $(POSTBUILD_MAKEFILE)
endif

# Helper rule
.PHONY : do_all
do_all : $(PROGRAM) $(POSTBUILD)

# REFERENCES
# [1]  http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# [2]  https://ftp.gnu.org/old-gnu/Manuals/make-3.79.1/html_chapter/make_10.html
# [3]  https://www.gnu.org/software/make/manual/html_node/Empty-Recipes.html
# [4]  https://stackoverflow.com/a/34983297/5051940
# [5]  https://www.gnu.org/software/make/manual/html_node/Static-Usage.html#Static-Usage

