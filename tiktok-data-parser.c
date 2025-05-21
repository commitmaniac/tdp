// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: Copyright (c) 2025 commitmaniac

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <cjson/cJSON.h>

int main() {
    FILE *file = fopen("user_data_tiktok.json", "r");
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

    char *result = cJSON_Print(activity);
    printf("%s\n", result);

    cJSON_free(json);
    return 0;
}
