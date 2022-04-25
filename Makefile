HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = playboxdemo.pdx

# Locate the SDK
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)

VPATH += extension
VPATH += playbox2d

# List C source files here
SRC = extension/main.c \
	playbox2d/array.c \
	playbox2d/maths.c \
	playbox2d/body.c \
	playbox2d/joint.c \
	playbox2d/collide.c \
	playbox2d/arbiter.c \
	playbox2d/world.c \
	playbox2d/playbox.c
	

# List all user directories here
UINCDIR = extension playbox2d

# List user asm files
UASRC = 

# List all user C define here, like -D_DEBUG=1
UDEFS = 

# Define ASM defines here
UADEFS = 

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

include $(SDK)/C_API/buildsupport/common.mk

# Make sure we compile a universal binary for M1 macs
DYLIB_FLAGS+=-arch x86_64 -arch arm64