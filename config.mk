# See LICENSE file for copyright and license details.
# minecurses version
VERSION = 0

# paths
PREFIX = /usr/local
MAN_DIR = ${PREFIX}/man/man6
BIN_DIR = ${PREFIX}/bin

# includes and libs
INCS = -Iinclude 
LIBS = -Llib -lncurses

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L \
	   -D_XOPEN_SOURCE=600 -DVERSION=\"${VERSION}\"
CFLAGS = -std=c99 -pedantic -Wall -Wno-deprecated-declarations \
	 -O3 ${INCS} ${CPPFLAGS}
LDFLAGS = ${LIBS}

# utils
CP = cp -f
RM = rm -f
RM_DIR = rm -rf
MV = mv
MKDIR = mkdir -p
RM_DIR = rm -rf
TAR = tar -cf
GZIP = gzip

# compiler
CC = gcc
