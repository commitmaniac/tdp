/*
  SPDX-License-Identifier: BSD-3-Clause
  SPDX-FileCopyrightText: Copyright (c) 2025 commitmaniac
*/

#ifdef CJSON_LOCAL
#include "cJSON.h"
#else
#include <cjson/cJSON.h>
#endif /* CJSON_LOCAL */

#define FLAG_IMPLEMENTATION
#include "flag.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef VERSION
char *VERSION;
#endif /* VERSION */

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} String_Builder;

typedef struct {
    char *base;
    char *items;
    char *result;
} TikTok_JSON_Keys;

void flag_program_usage(FILE *stream) {
    fprintf(stream, "Usage: %s [OPTIONS] PATH\n", flag_program_name());
    flag_print_options(stream);
}

bool read_entire_file(const char *path, String_Builder *sb) {
    bool result = true;

    FILE *file = fopen(path, "r");
    assert(file != NULL);
    if (fseek(file, 0, SEEK_END) < 0) result = false;

    long fsize = ftell(file);
    if (fsize < 0) result = false;
    if (fseek(file, 0, SEEK_SET) < 0) result = false;

    size_t new_count = sb->count + fsize;
    if (new_count > sb->capacity) {
        sb->items = realloc(sb->items, new_count);
        assert(sb->items != NULL);
        sb->capacity = new_count;
    }

    fread(sb->items + sb->count, fsize, 1, file);
    sb->count = new_count;

    if (file) fclose(file);
    return result;
}

int main(int argc, char **argv) {
    bool *help = flag_bool("help", false, "Print this help message");
    bool *saved = flag_bool("saved", false, "Parse and print saved videos");
    bool *liked = flag_bool("liked", false, "Parse and print liked videos");
    bool *history = flag_bool("history", false, "Parse and print video browsing history");
    bool *version = flag_bool("version", false, "Print installed version");

    if (!flag_parse(argc, argv)) {
        flag_program_usage(stderr);
        flag_print_error(stderr);
        return 1;
    }

    argc = flag_rest_argc();
    argv = flag_rest_argv();

    if (*help) {
        flag_program_usage(stderr);
        return 0;
    }

    if (*version) {
        printf("%s %s (cJSON %s)\n", flag_program_name(), VERSION, cJSON_Version());
        return 0;
    }

    if (argc <= 0) {
        fprintf(stderr, "Error: no data file provided\n");
        return 1;
    }

    char *path = argv[0];
    String_Builder sb = {0};

    if (!read_entire_file(path, &sb)) {
        fprintf(stderr, "Error: could not read provided data file\n");
        return 1;
    }

    cJSON *json = cJSON_Parse(sb.items);
    cJSON *activity = cJSON_GetObjectItem(json, "Activity");

    TikTok_JSON_Keys key;

    if (*saved) {
        key.base = "Favorite Videos";
        key.items = "FavoriteVideoList";
        key.result = "Link";
    } else if (*liked) {
        key.base = "Like List";
        key.items = "ItemFavoriteList";
        key.result = "link";
    } else if (*history) {
        key.base = "Video Browsing History";
        key.items = "VideoList";
        key.result = "Link";
    } else {
        flag_program_usage(stderr);
        fprintf(stderr, "Error: no valid option provided\n");
        return 1;
    }

    cJSON *base = cJSON_GetObjectItem(activity, key.base);
    cJSON *items = cJSON_GetObjectItem(base, key.items);
    cJSON *item;
    cJSON_ArrayForEach(item, items) {
        cJSON *result = cJSON_GetObjectItem(item, key.result);
        char *value = cJSON_GetStringValue(result);
        printf("%s\n", value);
    }

    cJSON_Delete(json);
    return 0;
}
