#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/config.h"
#include "ini_parser.c"
#include "env_reader.c"

// 全局状态
static bool initialized = false;
static char* cached_string_value = NULL; // 用于缓存字符串返回值

// 字符串转换函数
static int string_to_int(const char* str, int default_value) {
    if (!str || *str == '\0') return default_value;
    char* endptr;
    int value = (int)strtol(str, &endptr, 10);
    return (*endptr == '\0') ? value : default_value;
}

static bool string_to_bool(const char* str, bool default_value) {
    if (!str || *str == '\0') return default_value;

    // 转换为小写进行比较
    char* lower = strdup(str);
    if (!lower) return default_value;

    for (char* p = lower; *p; p++) {
        *p = tolower(*p);
    }

    bool result = default_value;
    if (strcmp(lower, "true") == 0 || strcmp(lower, "1") == 0 || strcmp(lower, "yes") == 0 || strcmp(lower, "on") == 0) {
        result = true;
    } else if (strcmp(lower, "false") == 0 || strcmp(lower, "0") == 0 || strcmp(lower, "no") == 0 || strcmp(lower, "off") == 0) {
        result = false;
    }

    free(lower);
    return result;
}

static double string_to_double(const char* str, double default_value) {
    if (!str || *str == '\0') return default_value;
    char* endptr;
    double value = strtod(str, &endptr);
    return (*endptr == '\0') ? value : default_value;
}

config_error_t config_init(const char* filename) {
    if (initialized) {
        config_cleanup();
    }

    initialized = true;

    // 如果指定了文件名，解析INI文件
    if (filename) {
        config_error_t result = parse_ini_file(filename);
        if (result != CONFIG_SUCCESS) {
            initialized = false;
            return result;
        }
    }

    return CONFIG_SUCCESS;
}

const char* config_get_string(const char* key, const char* default_value) {
    if (!initialized || !key) return default_value;

    // 优先从环境变量读取
    const char* env_value = get_env_value(key);
    if (env_value) {
        free(cached_string_value);
        cached_string_value = strdup(env_value);
        return cached_string_value ? cached_string_value : default_value;
    }

    // 从INI文件读取
    const char* ini_value = get_config_value(key);
    if (ini_value) {
        free(cached_string_value);
        cached_string_value = strdup(ini_value);
        return cached_string_value ? cached_string_value : default_value;
    }

    // 返回默认值
    free(cached_string_value);
    cached_string_value = default_value ? strdup(default_value) : NULL;
    return cached_string_value;
}

int config_get_int(const char* key, int default_value) {
    if (!initialized || !key) return default_value;

    // 优先从环境变量读取
    const char* env_value = get_env_value(key);
    if (env_value) {
        return string_to_int(env_value, default_value);
    }

    // 从INI文件读取
    const char* ini_value = get_config_value(key);
    if (ini_value) {
        return string_to_int(ini_value, default_value);
    }

    return default_value;
}

bool config_get_bool(const char* key, bool default_value) {
    if (!initialized || !key) return default_value;

    // 优先从环境变量读取
    const char* env_value = get_env_value(key);
    if (env_value) {
        return string_to_bool(env_value, default_value);
    }

    // 从INI文件读取
    const char* ini_value = get_config_value(key);
    if (ini_value) {
        return string_to_bool(ini_value, default_value);
    }

    return default_value;
}

double config_get_double(const char* key, double default_value) {
    if (!initialized || !key) return default_value;

    // 优先从环境变量读取
    const char* env_value = get_env_value(key);
    if (env_value) {
        return string_to_double(env_value, default_value);
    }

    // 从INI文件读取
    const char* ini_value = get_config_value(key);
    if (ini_value) {
        return string_to_double(ini_value, default_value);
    }

    return default_value;
}

void config_cleanup() {
    if (!initialized) return;

    free_config_entries();
    free(cached_string_value);
    cached_string_value = NULL;
    free(last_error);
    last_error = NULL;
    initialized = false;
}

const char* config_get_error_message() {
    return last_error ? last_error : "No error";
}
