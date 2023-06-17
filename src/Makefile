C_COMPILER = /usr/bin/gcc
DEBUG = /usr/bin/gdb
RM = /usr/bin/rm
SYS_INSTALL = /usr/bin/install
MKDIR = /usr/bin/mkdir

C_LDLIB = `pkg-config --libs gtk4`
C_HEADER = `pkg-config --cflags gtk4` -I.
C_ARGUMENT = -Wall

OUTFILE = snake
C_OBJECT = main.o snake_engine_v1.o snake-application.o snake-window.o
C_OBJECT += snake-internal.o

GRESOURCE_COMPILER = /usr/bin/glib-compile-resources
GRESOURCE_OUTFILE = snake.gresource
GRESOURCE_SOURCE = snake.gresource.xml

all: $(OUTFILE) $(GRESOURCE_OUTFILE)

run: $(OUTFILE) $(GRESOURCE_OUTFILE)
	$(OUTFILE)

debug: $(OUTFILE)
	$(DEBUG) $(OUTFILE)

$(OUTFILE): $(C_OBJECT)
	$(C_COMPILER) -o $@ $^ $(C_LDLIB) $(C_ARGUMENT)

$(C_OBJECT): %.o : %.c
	$(C_COMPILER) -c -o $@ $< $(C_HEADER) $(C_ARGUMENT)

$(GRESOURCE_OUTFILE): $(GRESOURCE_SOURCE)
	$(GRESOURCE_COMPILER) --target=$(GRESOURCE_OUTFILE) $(GRESOURCE_SOURCE)

install:
	$(MKDIR) -p $(INSTALL_SHARE)
	$(SYS_INSTALL) -c $(GRESOURCE_OUTFILE) $(INSTALL_SHARE)
	$(SYS_INSTALL) -c $(OUTFILE) $(INSTALL_BIN)

uninstall:
	$(RM) -v -f $(INSTALL_SHARE)/$(GRESOURCE_OUTFILE) $(INSTALL_BIN)/$(OUTFILE)

.PHONY: clean
clean:
	-$(RM) -v $(C_OBJECT) $(GRESOURCE_OUTFILE) $(OUTFILE)