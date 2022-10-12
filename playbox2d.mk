# -- Find out more about where this file is relative to the Makefile including it.
_RELATIVE_FILE_PATH := $(lastword $(MAKEFILE_LIST))
_RELATIVE_DIR := $(subst /$(notdir $(_RELATIVE_FILE_PATH)),,$(_RELATIVE_FILE_PATH))

ifeq ($(_RELATIVE_FILE_PATH), $(_RELATIVE_DIR))
	# -- This makefile is in the current folder.
	_RELATIVE_DIR := .
endif

# -- Add us as an include search folder only if it's not already there.
uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))
UINCDIR := $(call uniq, $(UINCDIR) $(_RELATIVE_DIR))

# -- Add our source files
SRC := $(SRC) \
	   $(_RELATIVE_DIR)/playbox2d/array.c \
	   $(_RELATIVE_DIR)/playbox2d/maths.c \
	   $(_RELATIVE_DIR)/playbox2d/body.c \
	   $(_RELATIVE_DIR)/playbox2d/joint.c \
	   $(_RELATIVE_DIR)/playbox2d/collide.c \
	   $(_RELATIVE_DIR)/playbox2d/arbiter.c \
	   $(_RELATIVE_DIR)/playbox2d/world.c \
	   $(_RELATIVE_DIR)/playbox2d/playbox2d.c
