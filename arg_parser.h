#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PARAM_COUNT 4
#define PARAM_REQ_COUNT 9

typedef struct params {

    char *param_full_name;
    char *param_short_name;
    char *param_desc;
    char* value;
    void *param_validator;

} params;

static void init();

static int are_params_valid(char* args[], int argc);

int parse_args(char* args[], int argc, char* interface, char* target_ip, char* target_mac, char* spoof_ip);
