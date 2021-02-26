# makefile - build system for gl-ks
#
#
# @author: Cade Brown <cade@kscript.org>


# -*- Config -*-

# kscript module name
NAME           := gl

# kscript directory
KS             ?= /usr/local

# install path
PREFIX         := /usr/local
DESTDIR        ?= 
TODIR          := $(DESTDIR)$(PREFIX)

# Add from the OpenGL configuration
CXXFLAGS       += 
LDFLAGS        += -lglfw
DEFS           += -DKSGL_GLFW

# Assimp
CXXFLAGS       += 
LDFLAGS        += -lassimp
DEFS           += -DKSGL_ASSIMP

# Add from the kscript configuration
CXXFLAGS       += -I$(KS)/include
LDFLAGS        += -L$(KS)/lib

# DEBUG
CXXFLAGS += -g


# -*- Files -*-

src_C          := $(wildcard src/*.c) $(wildcard src/glfw/*.c) $(wildcard src/ai/*.c) $(wildcard src/util/*.c)
src_H          := $(wildcard include/*.h)

src_O          := $(patsubst %.c,%.o,$(src_C))


# -*- Output -*-

# output shared object file
mod_SO         := ksm_$(NAME).so


# -*- Rules -*-

.PHONY: default all clean install uninstall FORCE


default: $(mod_SO)

all: $(mod_SO)

clean: FORCE
	rm -f $(wildcard $(src_O) $(mod_SO))

install: FORCE
	install -d $(TODIR)/lib/ks/pkgs/$(NAME)
	install -m 664 $(mod_SO) $(TODIR)/lib/ks/pkgs/$(NAME)/$(notdir $(mod_SO))
	strip $(TODIR)/lib/ks/pkgs/$(NAME)/$(notdir $(mod_SO))

uninstall: FORCE
	rm -rf $(TODIR)/lib/ks/pkgs/$(NAME)

FORCE:

$(mod_SO): $(src_O)
	$(CC) $(FPIC) \
		$^ \
		$(LDFLAGS) -shared -o $@

%.o: %.c $(src_HH)
	$(CC) $(CFLAGS) $(DEFS) -Iinclude -fPIC -c -o $@ $<

