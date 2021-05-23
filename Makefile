##
## Makefile for GUIslice library - LINUX
## - Calvin Hass
## - https://www.impulseadventure.com/elec/guislice-gui.html
## - https://github.com/ImpulseAdventure/GUIslice
##
## NOTE: This Makefile handles both SDL1.2 and SDL2 compilation.
##       A simpler Makefile can be used if this flexibility is
##       not required.

# ---------------------------------------------------------------------------
# Define compiler flags for GUIslice
# - In some cases, the makefile compilation options must match
#   settings in the GUIslice_config_linux.h configuration file.
#
# GRAPHICS DRIVER
# - make parameter: GSLC_DRV=(SDL1,SDL2)
# - On the makefile command-line, pass the GSLC_DRV parameter to select
#   the graphics driver matching the #define DRV_DISP_* setting in
#   GUIslice_config_linux.h, eg:
#     GUIslice_config_linux.h:  #define DRV_DISP_SDL1
#     Makefile:                 make <target> GSLC_DRV=SDL1
#
#
# TOUCH DRIVER
# - make parameter: GSLC_TOUCH=(TSLIB,SDL,NONE)
# - On the makefile command-line, pass the GSLC_TOUCH parameter to
#   indicate whether or not DRV_TOUCH_TSLIB is #defined in the
#   GUIslice_config_linux.h, eg:
#     GUIslice_config_linux.h:  #define DRV_TOUCH_TSLIB
#     Makefile:                 make <target> GSLC_TOUCH=TSLIB
#
#     GUIslice_config_linux.h:  #define DRV_TOUCH_SDL (or DRV_TOUCH_NONE)
#     Makefile:                 make <target> GSLC_TOUCH=NONE
# - Note that the makefile only checks to see if DRV_TOUCH=TSLIB and
#   will ignore any other values.
#
# NOTES:
# - Note that multiple parameters may be passed, for example:
#     make ex04_lnx_ctrls
#     make ex06_lnx_callback GSLC_TOUCH=TSLIB GSLC_DRV=SDL2
#     make all GSLC_TOUCH=TSLIB
#


DEBUG = -O3
CC = gcc
INCLUDE = -I/usr/local/include
CFLAGS = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -g
LDFLAGS = -L/usr/local/lib

GSLC_CORE = ../GUIslice/src/GUIslice.c ../GUIslice/src/GUIslice_config.h ../GUIslice/src/elem/*.c


# Define default compiler flags which may be overridden by command line

# Define graphics driver (overridden by command line GSLC_DRV)
GSLC_DRV := SDL1
#GSLC_DRV := SDL2

# Define any extra linker settings (overridden by command line GSLC_TOUCH_TSLIB)
#GSLC_LDLIB_EXTRA := -lm
#GSLC_LDLIB_EXTRA := -lm -lts

ifndef GSLC_TOUCH
  GSLC_LDLIB_EXTRA := -lm -lts
  $(info GUIslice touch mode: tslib enabled by default - GUIslice config should use DRV_TOUCH_TSLIB mode)
else
  ifeq (TSLIB,${GSLC_TOUCH})
    $(info GUIslice touch mode: tslib enabled - GUIslice config should use DRV_TOUCH_TSLIB mode)
    GSLC_LDLIB_EXTRA := -lm -lts
  else
    $(info GUIslice touch mode: tslib not enabled - GUIslice config should use DRV_TOUCH_SDL mode)
    GSLC_LDLIB_EXTRA := -lm
  endif
endif

# === SDL1.2 ===
ifeq (SDL1,${GSLC_DRV})
  $(info GUIslice driver mode: SDL1)
  GSLC_SRCS = ../GUIslice/src/GUIslice_drv_sdl.c
  # - Add extra linker libraries if needed
  LDLIBS = -lSDL -lSDL_ttf ${GSLC_LDLIB_EXTRA}
endif

# === SDL2.0 ===
ifeq (SDL2,${GSLC_DRV})
  $(info GUIslice driver mode: SDL2)
  GSLC_SRCS = ../GUIslice/src/GUIslice_drv_sdl.c
  # - Add extra linker libraries if needed
  LDLIBS = -lSDL2 -lSDL2_ttf ${GSLC_LDLIB_EXTRA}
endif

# === Adafruit-GFX ===
# No makefile for Arduino as most users will use the IDE for compilation

# ---------------------------------------------------------------------------

SRC = main.cpp

OBJ = $(SRC:.c=.o)

BINS = $(SRC:.c=)

all: $(BINS)

clean:
	@echo "Cleaning directory..."
	$(RM) $(BINS)

main: main.cpp $(GSLC_CORE) $(GSLC_SRCS)
	@echo [Building $@]
	@$(CC) $(CFLAGS) -o $@ main.cpp $(GSLC_CORE) $(GSLC_SRCS) $(LDFLAGS) $(LDLIBS) -I . -I ../GUIslice/src