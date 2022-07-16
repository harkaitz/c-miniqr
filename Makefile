## Configuration
DESTDIR    =
PREFIX     =/usr/local
AR         =ar
CC         =gcc
CFLAGS     =-Wall -g
PROGRAMS   =miniqr
LIBRARIES  =libminiqr.a
HEADERS    =miniqr.h
CFLAGS_ALL =$(LDFLAGS) $(CFLAGS) $(CPPFLAGS)

## Usual targets
all: $(PROGRAMS) $(LIBRARIES)

install: all
	install -d                  $(DESTDIR)$(PREFIX)/bin
	install -m755 $(PROGRAMS)   $(DESTDIR)$(PREFIX)/bin
	install -d                  $(DESTDIR)$(PREFIX)/include
	install -m644 $(HEADERS)    $(DESTDIR)$(PREFIX)/include
	install -d                  $(DESTDIR)$(PREFIX)/lib
	install -m644 $(LIBRARIES)  $(DESTDIR)$(PREFIX)/lib
clean:
	rm -f $(PROGRAMS) $(LIBRARIES)

## Library and program
libminiqr.a : miniqr.c $(HEADERS)
	$(CC) -c miniqr.c $(CFLAGS_ALL)
	$(AR) -crs $@ miniqr.o
	rm -f miniqr.o
miniqr: main.c libminiqr.a $(HEADERS)
	$(CC) -o $@ main.c libminiqr.a $(CFLAGS_ALL)

## -- manpages --
MAN_3=./miniqr.3 
install: install-man3
install-man3: $(MAN_3)
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man3
	cp $(MAN_3) $(DESTDIR)$(PREFIX)/share/man/man3
## -- manpages --
## -- license --
install: install-license
install-license: LICENSE
	mkdir -p $(DESTDIR)$(PREFIX)/share/doc/c-miniqr
	cp LICENSE $(DESTDIR)$(PREFIX)/share/doc/c-miniqr
## -- license --
