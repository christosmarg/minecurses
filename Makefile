# See LICENSE file for copyright and license details.
# minecurses - a terminal minesweeper game
.POSIX:

BIN = minecurses
VERSION = 0.1
DIST = ${BIN}-${VERSION}
MAN1 = ${BIN}.1
PREFIX = /usr/local
MAN_DIR = ${PREFIX}/man/man1
BIN_DIR = ${PREFIX}/bin

#EXT = c
#SRC = ${wildcard *.${EXT}}
#OBJ = ${SRC:%.${EXT}=%.o}
SRC = minecurses.c
OBJ = minecurses.o

CC = gcc
INCS = -Iinclude
CPPFLAGS = -DVERSION=\"${VERSION}\"
CFLAGS = -Wall -std=c99 -pedantic -O3 ${INCS} ${CPPFLAGS}
LDFLAGS = -Llib -lncurses -lSDL2 -lSDL2_mixer -lpthread

CP = cp -f
RM = rm -f
RM_DIR = rm -rf
MKDIR = mkdir -p
TAR = tar -cf
GZIP = gzip

all: options ${BIN}

options:
	@echo ${BIN} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

${BIN}: ${OBJ}
	${CC} ${LDFLAGS} ${OBJ} -o $@

${OBJ}: ${SRC} defs.h
	${CC} ${CFLAGS} -c ${SRC} -o $@

dist: clean
	${MKDIR} ${DIST}
	${CP} -R ${SRC} defs.h LICENSE Makefile log res ${DIST}
	${TAR} ${DIST}.tar ${DIST}
	${GZIP} ${DIST}.tar
	${RM_DIR} ${DIST}

run:
	./${BIN}

install: all
	#${MKDIR} ${DESTDIR}${BIN_DIR} ${DESTDIR}${MAN_DIR}
	${MKDIR} ${DESTDIR}${BIN_DIR}
	${CP} ${BIN} ${BIN_DIR}
	#${CP} ${MAN1} ${DESTDIR}${MAN_DIR}
	#sed "s/VERSION/${VERSION}/g" < ${MAN1} > ${DESTDIR}${MAN_DIR}/${MAN1}
	chmod 755 ${DESTDIR}${BIN_DIR}/${BIN}
	#chmod 644 ${DESTDIR}${MAN_DIR}/${MAN1}

uninstall:
	${RM} ${DESTDIR}${BIN_DIR}/${BIN}
	#${RM} ${DESTDIR}${MAN_DIR}/${MAN1}

clean:
	${RM} ${BIN} ${OBJ} ${DIST}.tar.gz

.PHONY: all options clean dist install uninstall run
