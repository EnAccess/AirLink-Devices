# default goal for when make is run by itself (`make` === `make clean all`)
cleanfirst: clean all

# default build has minimal dependencies (just requires GCC)
BUILD := default
# Use gnu11 instead of c99 for timer_t
C_STANDARD := gnu11
# Use g++20 to add support for designated initializers
CPP_STANDARD := gnu++20

# define compilation flags (includes, optimization, debug, warnings) here
INCLUDES := -Iinc -Isrc -I../.. -I../../include -I../../utils -I../../oc/ -I../../oc/api -I../../oc/include -I../../oc/messaging/coap -I../../oc/port -I../../oc/util
# Wno-comment and Wno-sign-compare for IoTivity files
WARNINGS.default := -Wall -Wextra -Wpedantic -Wno-type-limits -Wno-comment \
	-Wno-sign-compare -Wno-unused-variable -Wno-unknown-pragmas
WARNINGS.cpp := -Wall -Wextra -Wpedantic -Wno-type-limits -Wno-comment \
	-Wno-sign-compare -Wno-unused-variable -Wno-unknown-pragmas -Wno-pragmas
WARNINGS.sa := $(WARNINGS.default) -Werror -Wno-unused-parameter -Wundef \
    -Wuninitialized -Winit-self -Wswitch-default -Wunreachable-code \
    -Wlogical-op -Winline -Wsuggest-attribute=const

CFLAGS := $(INCLUDES) -O0 -g $(WARNINGS.$(BUILD)) -std=$(C_STANDARD)
CPPFLAGS := $(INCLUDES) -O0 -g $(WARNINGS.cpp) -std=$(CPP_STANDARD)

# add CFLAGS to any build except for cpp build
ifeq ($(BUILD),cpp)
  FLAGS=$(CPPFLAGS)
else
  FLAGS=$(CFLAGS)
endif

# Add '--status-bugs' flag to exit on first error
SCANBUILD_FLAGS := -enable-checker core\
                  -enable-checker deadcode\
                  -enable-checker alpha.core.BoolAssignment\
                  -enable-checker alpha.core.CallAndMessageUnInitRefArg\
                  -enable-checker alpha.core.CastSize\
                  -enable-checker alpha.core.CastToStruct\
                  -enable-checker alpha.core.FixedAddr\
                  -enable-checker alpha.core.IdenticalExpr\
                  -enable-checker alpha.core.PointerArithm\
                  -enable-checker alpha.core.PointerSub\
                  -enable-checker alpha.core.SizeofPtr\
                  -enable-checker alpha.core.TestAfterDivZero\
                  -enable-checker alpha.deadcode.UnreachableCode\
                  -enable-checker alpha.security.ArrayBoundV2\
                  -enable-checker alpha.security.ReturnPtrRange\
                  -enable-checker alpha.security.taint.TaintPropagation\
                  -analyze-headers\
                  -maxloop 40\
                  -o scanbuild-reports/

# default skips scan-build, run `make all BUILD=sa` to enable static analysis
CC.sa := scan-build $(SCANBUILD_FLAGS) gcc
CC.default := gcc
CC.cpp := g++-10
CC := $(CC.$(BUILD))

# Include math and RT libraries for `floor` and `timer_t` functions
LDFLAGS := -lm -lrt

# this rule defines how to get .o files from .c files
# when CC is overridden on the command line, this rule is executed directly
# because the definition of CC above is ignored (this is why FLAGS must be
# specified here rather than in CC in order for these builds to work)
%.o : %.c
	$(CC) -c $(FLAGS) $< -o $@

SOURCE_FILES := $(shell find src -name '*.c')

# These are the files from the Nexus library
LIB_OC_SOURCE_FILES := $(shell find ../../oc -name '*.c')
LIB_SOURCE_FILES := $(shell find ../../src -name '*.c')
LIB_UTIL_SOURCE_FILES := $(shell find ../../utils -name '*.c')

# take every c file, and replace the ".c" with ".o" (list of all object files)
SRC_OBJECT_FILES := $(SOURCE_FILES:.c=.o)
LIB_OC_SOURCE_OBJECT_FILES := $(LIB_OC_SOURCE_FILES:.c=.o)
LIB_OBJECT_FILES := $(LIB_SOURCE_FILES:.c=.o)
LIB_UTIL_SOURCE_OBJECT_FILES := $(LIB_UTIL_SOURCE_FILES:.c=.o)

OBJECT_FILES := $(SRC_OBJECT_FILES) $(LIB_OC_SOURCE_OBJECT_FILES) $(LIB_OBJECT_FILES) $(LIB_UTIL_SOURCE_OBJECT_FILES)

PROGRAM_NAME := sample_nexus_keycode_program

# link all the object files in the project
all: $(OBJECT_FILES)
	$(CC) -o $(PROGRAM_NAME) $(OBJECT_FILES) $(LDFLAGS)

install-dependencies:
	sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
	sudo apt-get update -y
	sudo apt-get install gcc-9 -y
	sudo apt-get install g++-10 -y
	sudo apt-get install clang-6.0 -y
	sudo apt-get install clang-tools-6.0 -y
	sudo apt-get install clang-tidy-6.0 -y
	sudo apt-get install clang-format-6.0 -y
	sudo apt-get install check -y

# 'phony' target; always execute clean regardless of file state
.PHONY: clean

# Includes for stdbool.h and stddef.h
# Used in `tidy` below, adjust path as required.
CLANG_INCLUDE := -I/usr/lib/llvm-6.0/lib/clang/6.0.0/include

tidy:
	clang-tidy $(SOURCE_FILES) -checks=* -- $(INCLUDES) $(CLANG_INCLUDE)

format:
	$(foreach file, $(SOURCE_FILES), clang-format-6.0 -style=file -i $(file);)

clean:
	rm -rf $(OBJECT_FILES)
	rm -f $(PROGRAM_NAME)
