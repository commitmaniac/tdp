tdp: tiktok-data-parser.c
	clang -o $@ $< -lcjson

.PHONY: clean

clean:
	rm -rf tdp
