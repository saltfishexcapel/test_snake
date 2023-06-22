C_COMPILER = /usr/bin/gcc
DEBUG = /usr/bin/gdb
RM = /usr/bin/rm
SYS_INSTALL = /usr/bin/install
MKDIR = /usr/bin/mkdir

C_LDLIB = `pkg-config --libs gtk4`
C_HEADER = `pkg-config --cflags gtk4` -I.
C_ARGUMENT = -Wall -g

OUTFILE = snake
C_OBJECT = main.o snake_engine_v1.o snake-application.o snake-window.o
C_OBJECT += snake-internal.o
C_OBJECT += snake-property.o

GRESOURCE_COMPILER = /usr/bin/glib-compile-resources
GRESOURCE_OUTFILE = snake.gresource
GRESOURCE_SOURCE = snake.gresource.xml

TEST_OUTFILE = a.out
TEST_OBJECT = test.o

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
	$(MKDIR) -p $(INSTALL_APPPATH)
	$(SYS_INSTALL) -c $(GRESOURCE_OUTFILE) $(INSTALL_APPPATH)
	$(SYS_INSTALL) -c $(OUTFILE) $(INSTALL_BIN)

uninstall:
	$(RM) -v -i -f $(INSTALL_APPPATH)/$(GRESOURCE_OUTFILE) $(INSTALL_BIN)/$(OUTFILE)

test_debug: $(TEST_OUTFILE)
	$(DEBUG) $(TEST_OUTFILE)

$(TEST_OUTFILE): $(TEST_OBJECT)
	$(C_COMPILER) -o $@ $^ -Wall -g

$(TEST_OBJECT): %.o : %.c
	$(C_COMPILER) -c -o $@ $< -Wall -g -I.

.PHONY: clean
clean:
	-$(RM) -v -f $(C_OBJECT) $(GRESOURCE_OUTFILE) $(OUTFILE)
	-$(RM) -v -f $(TEST_OBJECT) $(TEST_OUTFILE)