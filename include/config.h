#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

// 错误码定义
typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_FILE_NOT_FOUND,
    CONFIG_ERROR_PARSE_ERROR,
    CONFIG_ERROR_MEMORY_ERROR,
    CONFIG_ERROR_KEY_NOT_FOUND
} config_error_t;

// 初始化配置系统，从环境变量和INI文件加载
// filename: INI配置文件名，如果为NULL则只从环境变量读取
config_error_t config_init(const char* filename);

// 获取字符串配置值
// 返回值需要调用者管理内存
const char* config_get_string(const char* key, const char* default_value);

// 获取整数配置值
int config_get_int(const char* key, int default_value);

// 获取布尔配置值
bool config_get_bool(const char* key, bool default_value);

// 获取浮点数配置值
double config_get_double(const char* key, double default_value);

// 清理配置系统，释放所有资源
void config_cleanup();

// 获取最后一次错误信息
const char* config_get_error_message();

#endif // CONFIG_H
