ifneq (,$(wildcard .git))
VERSION ?= $(shell git describe --tags)
else
VERSION ?= 1.0.1
endif

ifeq ($(STATIC),1)
CJSON_CFLAGS ?= -DCJSON_LOCAL
CJSON_LIBS   ?= -L./ -lcjson -static
else
CJSON_CFLAGS ?= $(shell pkg-config --cflags libcjson)
CJSON_LIBS   ?= $(shell pkg-config --libs libcjson)
endif

SRC := tiktok-data-parser.c
BIN ?= tdp

ifeq ($(STATIC),1)
all: libcjson.a $(BIN)
else
all: $(BIN)
endif

$(BIN): $(SRC)
	$(CC) -o $@ $< -DVERSION=\"$(VERSION)\" $(CJSON_CFLAGS) $(CJSON_LIBS)

cJSON.o: cJSON.c
	$(CC) -c -o $@ $<

libcjson.a: cJSON.o
	$(AR) rcs $@ $<

.PHONY: clean

clean:
	rm -rf *.o *.a $(BIN)
