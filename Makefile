CC      ?= cc
PREFIX  ?= /usr
BINDIR  ?= $(PREFIX)/bin
SYSCONFDIR ?= /etc

PKGS = webkitgtk-6.0

CFLAGS ?= -O2 -pipe
override CFLAGS  += -Wall -Wextra $(shell pkg-config --cflags $(PKGS))
override LDFLAGS += $(shell pkg-config --libs $(PKGS))

BIN     = vimb-blocklist-compile
SCRIPT  = vimb-blocklist-refresh

all: $(BIN)

$(BIN): $(BIN).c
	$(CC) $(CFLAGS) $(BIN).c -o $@ $(LDFLAGS)

install: $(BIN) $(SCRIPT)
	install -Dm755 $(BIN)    $(DESTDIR)$(BINDIR)/$(BIN)
	install -Dm755 $(SCRIPT) $(DESTDIR)$(SYSCONFDIR)/cron.weekly/$(SCRIPT)

clean:
	rm -f $(BIN)

.PHONY: all install clean
