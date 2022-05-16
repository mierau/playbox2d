HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = playboxdemo.pdx

# Locate the SDK
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)

# List all user directories here
UINCDIR =

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

include playbox2d/playbox2d.mk
include extension/extension.mk

include $(SDK)/C_API/buildsupport/common.mk

# Make sure we compile for x86 on M1 macs (until simulator supports them)
#DYLIB_FLAGS+=-arch x86_64