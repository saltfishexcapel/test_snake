C_COMPILER = /usr/bin/gcc
C_ARGUMENTS = -Wall

export INSTALL_PATH = /usr
export INSTALL_SHARE = $(INSTALL_PATH)/share
export INSTALL_APPPATH = $(INSTALL_SHARE)/test_snake
export INSTALL_BIN = $(INSTALL_PATH)/bin

MKDIR = /usr/bin/mkdir
RM = /usr/bin/rm

SNAKE_CONFIG = src/snake-config.h

CONFIG_TOOL = config.out
CONFIG_OBJECT = config.o

GRESOURCE_OUTFILE = snake.gresource

all: $(SNAKE_CONFIG)
	cd src && $(MAKE)
	
$(SNAKE_CONFIG): $(CONFIG_TOOL)
	$(CONFIG_TOOL) $(SNAKE_CONFIG) $(INSTALL_APPPATH)/$(GRESOURCE_OUTFILE)

$(CONFIG_TOOL): $(CONFIG_OBJECT)
	$(C_COMPILER) -o $@ $^ $(C_ARGUMENTS)

$(CONFIG_OBJECT): %.o : %.c
	$(C_COMPILER) -c -o $@ $^ $(C_ARGUMENTS)

install:
	cd src && $(MAKE) install
	cd data && $(MAKE) install

uninstall:
	cd src && $(MAKE) uninstall
	cd src && $(MAKE) uninstall

test:
	cd src && $(MAKE) test_debug

.PHONY: clean
clean:
	$(RM) -v -f $(SNAKE_CONFIG) $(CONFIG_OBJECT) $(CONFIG_TOOL)
	cd src && $(MAKE) clean