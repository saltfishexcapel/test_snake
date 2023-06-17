MKDIR = /usr/bin/mkdir
SYS_INSTALL = /usr/bin/install
RM = /usr/bin/rm

DESKTOP = snake.desktop
ICONS = icon/snake.png

INSTALL_APPLICATION = $(INSTALL_SHARE)/applications

install:
	$(MKDIR) -p $(INSTALL_APPPATH)
	$(SYS_INSTALL) -c $(ICONS) $(INSTALL_APPPATH)
	$(SYS_INSTALL) -c $(DESKTOP) $(INSTALL_APPLICATION)

uninstall:
	$(RM) -v -f $(INSTALL_APPLICATION)/$(DESKTOP)