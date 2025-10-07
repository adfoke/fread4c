#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 获取环境变量值
// 支持大小写不敏感的查找
const char* get_env_value(const char* key) {
    if (!key) return NULL;

    // 首先尝试直接的key
    const char* value = getenv(key);
    if (value) return value;

    // 如果没找到，尝试转换为大写
    char* upper_key = strdup(key);
    if (!upper_key) return NULL;

    for (char* p = upper_key; *p; p++) {
        *p = toupper(*p);
    }

    value = getenv(upper_key);
    free(upper_key);
    return value;
}

// 检查环境变量是否存在
int env_exists(const char* key) {
    return get_env_value(key) != NULL;
}
