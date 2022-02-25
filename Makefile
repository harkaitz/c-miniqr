## Configuration
DESTDIR    =
PREFIX     =/usr/local
AR         =ar
CC         =gcc
CFLAGS     =-Wall -g
CPPFLAGS   =
LIBS       =

## Sources and targets
PROGRAMS   =miniqr
LIBRARIES  =libminiqr.a
HEADERS    =miniqr.h
MARKDOWNS  =miniqr.3.md
MANPAGES_3 =miniqr.3
SOURCES_L  =miniqr.c
SOURCES_P  =main.c

## AUXILIARY
SOURCES    =$(SOURCES_L) $(SOURCES_P)
CFLAGS_ALL =$(LDFLAGS) $(CFLAGS) $(CPPFLAGS)

## STANDARD TARGETS
all: $(PROGRAMS) $(LIBRARIES)
help:
	@echo "all     : Build everything."
	@echo "clean   : Clean files."
	@echo "install : Install all produced files."
install: all
	install -d                  $(DESTDIR)$(PREFIX)/bin
	install -m755 $(PROGRAMS)   $(DESTDIR)$(PREFIX)/bin
	install -d                  $(DESTDIR)$(PREFIX)/include
	install -m644 $(HEADERS)    $(DESTDIR)$(PREFIX)/include
	install -d                  $(DESTDIR)$(PREFIX)/lib
	install -m644 $(LIBRARIES)  $(DESTDIR)$(PREFIX)/lib
	install -d                  $(DESTDIR)$(PREFIX)/share/man/man3
	install -m644 $(MANPAGES_3) $(DESTDIR)$(PREFIX)/share/man/man3
clean:
	rm -f $(PROGRAMS) $(LIBRARIES)
ssnip:
	ssnip LICENSE $(MARKDOWNS) $(HEADERS) $(SOURCES) $(MANPAGES_3)

## TARGETS
libminiqr.a : $(SOURCES_L) $(HEADERS)
	$(CC) -c miniqr.c $(CFLAGS_ALL)
	$(AR) -crs $@ miniqr.o
	rm -f miniqr.o
miniqr: main.c libminiqr.a $(HEADERS)
	$(CC) -o $@ main.c libminiqr.a $(CFLAGS_ALL)
