ifneq (,$(wildcard .git))
VERSION ?= $(shell git describe --tags)
else
VERSION ?= 1.0.0
endif

tdp: tiktok-data-parser.c
	clang -o $@ $< -lcjson -DVERSION=\"$(VERSION)\"

.PHONY: clean

clean:
	rm -rf tdp
