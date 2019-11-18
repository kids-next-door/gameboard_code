#
#  rpi.mk
#
#  If you are using makefile `dep-makefile.mk' to build your code and you want
#  to cross compile that code to produced executable code for the Raspberry Pi,
#  create a hidden file named '.rpi' within the source code folder to enable
#  cross compiling for the Raspberry Pi. This also enables linking with the
#  wiringPi library.
#
#  2019-04-02 Jim Fischer <fischerjd@missouri.edu>
#

# The program name
PROGRAM := project


# Cross compiler information
RPICROSS_BASEDIR := /usr/local/rpicross
RPICROSS_BINDIR  := $(RPICROSS_BASEDIR)/bin
RPICROSS_PREFIX  := arm-linux-gnueabihf-

# If we're building on a computer where the directory `$(RPICROSS_BINDIR)'
# exists, assume we're cross compiling and therefore redefine make variables CC
# and CXX to use the cross C/C++ compilers and not the native C/C++ compilers.
# 
# Also, create a make variable `RPIFS' whose value is the path to the folder
# `rpifs' within the user's HOME directory on their desktop computer, which is
# presumably the folder where the RPI's file system is currently mounted.  (See
# also: man mount.rpifs).
#
# If folder $(RPICROSS_BINDIR) does not exist, assume we're building on the RPi
# itself.  In this case the make variables CC and CXX retain their original
# values and the make variable RPIFS is undefined. (n.b. The expansion of an
# undefined make variable is the null string "".)
ifneq ($(wildcard $(RPICROSS_BINDIR)),)
	CC := $(RPICROSS_BINDIR)/$(RPICROSS_PREFIX)gcc
	CXX := $(RPICROSS_BINDIR)/$(RPICROSS_PREFIX)g++
	RPIFS := $(HOME)/rpifs
endif

# C preprocessor flags
# Specify the path to the folder where the wiringPi library's header files are
# stored.
CPPFLAGS += -I$(RPIFS)/usr/local/include

# GCC linker/loader (ld) flags
# -L Specify the path to the folder that contains the library file
#  `libwiringPi.so.*'.
# -l Specify the name of the library we want to link our program to.  The
#  wiringPi library's file name is `libwiringPi.so'; therefore, the library's
#  name is `wiringPi' (strip off the `lib' prefix and the `.so.*' suffix).
LDFLAGS += -L$(RPIFS)/usr/local/lib
LDLIBS += -lwiringPi

