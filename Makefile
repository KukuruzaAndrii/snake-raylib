PROJECT_NAME          ?= snake

SRC_DIR               := src
BUILD_DIR             := build
BIN_DIR               := bin

GAME_BIN              := $(BIN_DIR)/$(PROJECT_NAME)
SRC                   := $(wildcard $(SRC_DIR)/*.c)
HDRS                  := $(wildcard $(SRC_DIR)/*.h)
OBJ                   := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
$(info OBJ = $(OBJ))

all: run

cmpl: $(GAME_BIN)

run: cmpl
	./$(GAME_BIN)

debug: CFLAGS += -g -D_DEBUG
debug: cmpl
	gdb ./$(GAME_BIN)

$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

$(GAME_BIN): $(OBJ) | $(BIN_DIR)
	$(CC) -o $(GAME_BIN)$(EXT) $(OBJ) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS) | $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(BUILD_DIR)


PLATFORM              ?= PLATFORM_DESKTOP
PLATFORM_OS           := LINUX

RAYLIB_INCLUDE_PATH   ?= /usr/local/include
RAYLIB_LIB_PATH       ?= /usr/local/lib
RAYLIB_LIBTYPE        ?= STATIC

# Build mode for project: DEBUG or RELEASE
BUILD_MODE            ?= RELEASE

# Use Wayland display server protocol on Linux desktop (by default it uses X11 windowing system)
# NOTE: This variable is only used for PLATFORM_OS: LINUX
USE_WAYLAND_DISPLAY   ?= FALSE

# Define compiler flags: CFLAGS
#------------------------------------------------------------------------------------------------
#  -O1                  defines optimization level
#  -g                   include debug information on compilation
#  -s                   strip unnecessary data from build
#  -Wall                turns on most, but not all, compiler warnings
#  -std=c99             defines C language mode (standard C from 1999 revision)
#  -std=gnu99           defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces  ignore invalid warning (GCC bug 53119)
#  -Wno-unused-value    ignore unused return values of some functions (i.e. fread())
#  -D_DEFAULT_SOURCE    use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
CFLAGS = -std=c99 -Werror -Wall -Wextra -Wno-missing-braces -Wunused-result -D_DEFAULT_SOURCE

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -D_DEBUG
endif

# Additional flags for compiler (if desired)
#CFLAGS += -Wextra -Wmissing-prototypes -Wstrict-prototypes
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),LINUX)
        ifeq ($(RAYLIB_LIBTYPE),STATIC)
            CFLAGS += -D_DEFAULT_SOURCE
        endif
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            # Explicitly enable runtime link to libraylib.so
            CFLAGS += -Wl,-rpath,$(RAYLIB_RELEASE_PATH)
        endif
    endif
endif

# Define include paths for required headers: INCLUDE_PATHS
# NOTE: Some external/extras libraries could be required (stb, physac, easings...)
#------------------------------------------------------------------------------------------------

# Define additional directories containing required header files
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),LINUX)
        INCLUDE_PATHS += -I$(RAYLIB_INCLUDE_PATH)
    endif
endif

# Define library paths containing required libs: LDFLAGS
#------------------------------------------------------------------------------------------------

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),LINUX)
        LDFLAGS += -L$(RAYLIB_LIB_PATH)
    endif
endif


# Define libraries required on linking: LDLIBS
# NOTE: To link libraries (lib<name>.so or lib<name>.a), use -l<name>
#------------------------------------------------------------------------------------------------
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),LINUX)
        # Libraries for Debian GNU/Linux desktop compiling
        # NOTE: Required packages: libegl1-mesa-dev
        LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt

        # On X11 requires also below libraries
        LDLIBS += -lX11
        # NOTE: It seems additional libraries are not required any more, latest GLFW just dlopen them
        #LDLIBS += -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor

        # On Wayland windowing system, additional libraries requires
        ifeq ($(USE_WAYLAND_DISPLAY),TRUE)
            LDLIBS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
        endif
        # Explicit link to libc
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LDLIBS += -lc
        endif
    endif
endif

