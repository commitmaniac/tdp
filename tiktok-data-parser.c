// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: Copyright (c) 2025 commitmaniac

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <cjson/cJSON.h>

#define FLAG_IMPLEMENTATION
#include "flag.h"

#ifndef VERSION
char *VERSION;
#endif // VERSION

typedef struct {
    char *base;
    char *items;
    char *result;
} TikTok_JSON_Keys;

void usage(FILE *stream, const char *prog) {
    fprintf(stream, "Usage: %s [OPTIONS] PATH\n", prog);
    flag_print_options(stream);
}

int main(int argc, char **argv) {
    bool *help = flag_bool("help", false, "Print this help message");
    bool *saved = flag_bool("saved", false, "Parse and print saved videos");
    bool *liked = flag_bool("liked", false, "Parse and print liked videos");
    bool *version = flag_bool("version", false, "Print installed version");

    if (!flag_parse(argc, argv)) {
        usage(stderr, flag_program_name());
        flag_print_error(stderr);
        exit(1);
    }

    if (*help) {
        usage(stdout, flag_program_name());
        exit(0);
    }

    if (*version) {
        printf("%s %s\n", flag_program_name(), VERSION);
        exit(0);
    }

    int rest_argc = flag_rest_argc();
    char **rest_argv = flag_rest_argv();

    if (rest_argc <= 0) {
        fprintf(stderr, "Error: no input file provided\n");
        exit(1);
    }

    char *filename = NULL;
    for (int i = 0; i < rest_argc; ++i) {
        filename = rest_argv[i];
        break;
    }

    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    fseeko(file, 0, SEEK_END);
    long f_size = ftello(file);
    fseeko(file, 0, SEEK_SET);

    char *buff = malloc(f_size + 1);
    assert(buff != NULL);

    fread(buff, 1, f_size, file);
    buff[f_size] = '\0';

    fclose(file);

    cJSON *json = cJSON_Parse(buff);
    cJSON *activity = cJSON_GetObjectItem(json, "Activity");

    TikTok_JSON_Keys key;

    if (*saved) {
        key.base = "Favorite Videos";
        key.items = "FavoriteVideoList";
        key.result = "Link";
    }

    if (*liked) {
        key.base = "Like List";
        key.items = "ItemFavoriteList";
        key.result = "link";
    }

    cJSON *base = cJSON_GetObjectItem(activity, key.base);
    cJSON *items = cJSON_GetObjectItem(base, key.items);
    cJSON *video = NULL;
    cJSON_ArrayForEach(video, items) {
        cJSON *link = cJSON_GetObjectItem(video, key.result);
        char *result = cJSON_GetStringValue(link);
        printf("%s\n", result);
    }

    cJSON_free(json);
    return 0;
}
