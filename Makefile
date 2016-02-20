# C++ Makefile
# Bălan Mihail <mihail.balan@gmail.com>

# WARNING
# Does not work with filenames containing whitespaces

# Usage:
# make [rule] [build={debug|relase}]

NAME := bolov

SOURCES :=

PREFIX := /home/pk/opt/bolov

optimization_flags.debug   :=
optimization_flags.release := -flto -O3

warning_flags.all := -Wall -Wextra -Werror
warning_flags.debug := -Wno-error=unused-variable \
                       -Wno-error=unused-function \
                       -Wno-error=unused-value \
                       -Wno-error=unused-parameter \
                       -Wno-error=unused-local-typedefs
warning_flags.release :=

CXX := g++

CXXFLAGS := -std=c++14 -g
# cxxflags.debug =
# cxxflags.release =

LDFLAGS :=
# ldflags.debug :=
# ldflags.release :=

DEBUGGER := gdb -tui

include Makefile.d/MakefileLibFooter

include Makefile.d/BatchMakefile
