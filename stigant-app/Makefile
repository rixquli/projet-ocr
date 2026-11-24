CC      = gcc
LIBS_NAME    = gtk+-3.0 fontconfig
CFLAGS  = -Wall -Isrc $(shell pkg-config --cflags $(LIBS_NAME)) \
          -DFONT_DIR=\"$(CURDIR)/src/assets/fonts/\"
LIBS    = $(shell pkg-config --libs $(LIBS_NAME)) -lm

SRC = src/main.c \
      src/style/stigant-style.c \
      src/window/main/stigant-window.c \
	  src/window/main/stigant-controller.c \
	  src/window/main/stigant-model.c \
	  src/window/generate/stigant-generate.c \
	  src/window/generate/stigant-generate-controller.c \
	  src/window/generate/stigant-generate-model.c \
	  src/window/solver/stigant-solver.c \
	  src/window/solver/stigant-solver-model.c \
	  src/window/solver/stigant-solver-controller.c \
      src/widgets/stigant-grid-builder.c

HDR = $(shell find src -name '*.h')

RES_XML  = src/assets/stigant.gresource.xml
RES_C    = src/build/stigant-resources.c
RES_DEPS = $(shell glib-compile-resources --sourcedir=src/assets --generate-dependencies $(RES_XML))

stigant: $(SRC) $(HDR) $(RES_C)
	$(CC) $(CFLAGS) $(SRC) $(RES_C) -o $@ $(LIBS)

$(RES_C): $(RES_XML) $(RES_DEPS)
	@mkdir -p $(dir $@)
	glib-compile-resources --sourcedir=src/assets --target=$@ --generate-source $<

run: stigant
	./stigant

clean:
	rm -rf stigant $(dir $(RES_C))

.PHONY: run clean
