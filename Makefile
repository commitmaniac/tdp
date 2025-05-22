ifneq (,$(wildcard .git))
VERSION ?= $(shell git describe --tags)
endif

tdp: tiktok-data-parser.c
	clang -o $@ $< -lcjson -DVERSION=\"$(VERSION)\"

.PHONY: clean

clean:
	rm -rf tdp
