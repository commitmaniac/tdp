ifneq (,$(wildcard .git))
VERSION ?= $(shell git describe --tags)
else
VERSION ?= 1.0.0
endif

CJSON_CFLAGS ?= $(shell pkg-config --cflags libcjson)
CJSON_LIBS   ?= $(shell pkg-config --libs libcjson)

tdp: tiktok-data-parser.c
	$(CC) -o $@ $< -DVERSION=\"$(VERSION)\" $(CJSON_CFLAGS) $(CJSON_LIBS)

.PHONY: clean

clean:
	rm -rf tdp
