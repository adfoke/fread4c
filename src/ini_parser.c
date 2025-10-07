#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/config.h"

// 配置项结构体
typedef struct config_entry {
    char* section;
    char* key;
    char* value;
    struct config_entry* next;
} config_entry_t;

// 全局配置链表头
static config_entry_t* config_head = NULL;
static char* last_error = NULL;

// 设置错误信息
static void set_error(const char* message) {
    free(last_error);
    last_error = strdup(message);
}

// 释放配置链表
static void free_config_entries() {
    config_entry_t* current = config_head;
    while (current) {
        config_entry_t* next = current->next;
        free(current->section);
        free(current->key);
        free(current->value);
        free(current);
        current = next;
    }
    config_head = NULL;
}

// 添加配置项到链表
static config_error_t add_config_entry(const char* section, const char* key, const char* value) {
    config_entry_t* entry = malloc(sizeof(config_entry_t));
    if (!entry) {
        set_error("Memory allocation failed");
        return CONFIG_ERROR_MEMORY_ERROR;
    }

    entry->section = section ? strdup(section) : NULL;
    entry->key = strdup(key);
    entry->value = strdup(value);
    entry->next = NULL;

    if (!entry->key || !entry->value || (section && !entry->section)) {
        free(entry->section);
        free(entry->key);
        free(entry->value);
        free(entry);
        set_error("Memory allocation failed");
        return CONFIG_ERROR_MEMORY_ERROR;
    }

    // 添加到链表尾部
    if (!config_head) {
        config_head = entry;
    } else {
        config_entry_t* current = config_head;
        while (current->next) {
            current = current->next;
        }
        current->next = entry;
    }

    return CONFIG_SUCCESS;
}

// 查找配置值
static const char* find_config_value(const char* section, const char* key) {
    config_entry_t* current = config_head;
    while (current) {
        if ((section == NULL && current->section == NULL) ||
            (section && current->section && strcmp(section, current->section) == 0)) {
            if (strcmp(key, current->key) == 0) {
                return current->value;
            }
        }
        current = current->next;
    }
    return NULL;
}

// 解析INI文件
config_error_t parse_ini_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        set_error("Failed to open config file");
        return CONFIG_ERROR_FILE_NOT_FOUND;
    }

    char line[1024];
    char* current_section = NULL;

    while (fgets(line, sizeof(line), file)) {

        // 移除换行符
        char* newline = strchr(line, '\n');
        if (newline) *newline = '\0';
        newline = strchr(line, '\r');
        if (newline) *newline = '\0';

        // 跳过空行和注释
        char* trimmed = line;
        while (*trimmed && isspace(*trimmed)) trimmed++;
        if (*trimmed == '\0' || *trimmed == ';' || *trimmed == '#') {
            continue;
        }

        // 解析section
        if (*trimmed == '[') {
            char* end = strchr(trimmed, ']');
            if (!end) {
                fclose(file);
                set_error("Invalid section format");
                return CONFIG_ERROR_PARSE_ERROR;
            }
            *end = '\0';
            free(current_section);
            current_section = strdup(trimmed + 1);
            if (!current_section) {
                fclose(file);
                set_error("Memory allocation failed");
                return CONFIG_ERROR_MEMORY_ERROR;
            }
            continue;
        }

        // 解析key=value
        char* equals = strchr(trimmed, '=');
        if (!equals) {
            fclose(file);
            set_error("Invalid key-value format");
            return CONFIG_ERROR_PARSE_ERROR;
        }
        *equals = '\0';
        char* key = trimmed;
        char* value = equals + 1;

        // 移除key和value的空格
        while (*key && isspace(*key)) key++;
        char* key_end = key + strlen(key) - 1;
        while (key_end > key && isspace(*key_end)) {
            *key_end = '\0';
            key_end--;
        }

        while (*value && isspace(*value)) value++;
        char* value_end = value + strlen(value) - 1;
        while (value_end > value && isspace(*value_end)) {
            *value_end = '\0';
            value_end--;
        }

        if (*key == '\0') {
            fclose(file);
            set_error("Empty key");
            return CONFIG_ERROR_PARSE_ERROR;
        }

        config_error_t result = add_config_entry(current_section, key, value);
        if (result != CONFIG_SUCCESS) {
            fclose(file);
            return result;
        }
    }

    fclose(file);
    free(current_section);
    return CONFIG_SUCCESS;
}

// 获取配置值（内部使用）
const char* get_config_value(const char* key) {
    // 首先尝试无section的key
    const char* value = find_config_value(NULL, key);
    if (value) return value;

    // 搜索所有section（后定义的section优先）
    config_entry_t* current = config_head;
    while (current) {
        if (current->section && strcmp(key, current->key) == 0) {
            value = current->value;
        }
        current = current->next;
    }

    return value;
}
