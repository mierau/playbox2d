HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = playboxdemo.pdx

# Locate the SDK
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)

# List C source files here
SRC = extension/main.c

# List all user directories here
UINCDIR = extension

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

# Include sub makefiles
include playbox2d.mk
include $(SDK)/C_API/buildsupport/common.mk

# Make sure we compile a universal binary for M1 macs
DYLIB_FLAGS+=-arch x86_64 -arch arm64
