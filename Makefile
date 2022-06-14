SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o

# PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

build:
	gcc `pkg-config --cflags --libs glib-2.0,libportal` src/*.c -o dynamic-launcher

install:
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 dynamic-launcher $(DESTDIR)$(PREFIX)/bin/
