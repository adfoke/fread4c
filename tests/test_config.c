#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/config.h"

void test_env_variables() {
    printf("Testing environment variables...\n");

    // 设置环境变量用于测试
    setenv("TEST_STRING", "hello world", 1);
    setenv("TEST_INT", "42", 1);
    setenv("TEST_BOOL_TRUE", "true", 1);
    setenv("TEST_BOOL_FALSE", "false", 1);
    setenv("TEST_DOUBLE", "3.14", 1);

    // 初始化（不使用配置文件）
    config_error_t result = config_init(NULL);
    assert(result == CONFIG_SUCCESS);

    // 测试字符串
    const char* str = config_get_string("TEST_STRING", "default");
    assert(strcmp(str, "hello world") == 0);

    // 测试整数
    int num = config_get_int("TEST_INT", 0);
    assert(num == 42);

    // 测试布尔值
    bool bool_true = config_get_bool("TEST_BOOL_TRUE", false);
    assert(bool_true == true);

    bool bool_false = config_get_bool("TEST_BOOL_FALSE", true);
    assert(bool_false == false);

    // 测试浮点数
    double dbl = config_get_double("TEST_DOUBLE", 0.0);
    assert(dbl > 3.13 && dbl < 3.15);

    // 测试默认值
    const char* default_str = config_get_string("NON_EXISTENT", "default_value");
    assert(strcmp(default_str, "default_value") == 0);

    printf("Environment variables test passed!\n");
}

void test_ini_file() {
    printf("Testing INI file parsing...\n");

    // 创建测试INI文件
    FILE* file = fopen("test_config.ini", "w");
    if (!file) {
        printf("Failed to create test config file\n");
        return;
    }

    fprintf(file, "# Test configuration file\n");
    fprintf(file, "[app]\n");
    fprintf(file, "name = myapp\n");
    fprintf(file, "version = 1.0\n");
    fprintf(file, "debug = true\n");
    fprintf(file, "\n");
    fprintf(file, "[database]\n");
    fprintf(file, "host=localhost\n");
    fprintf(file, "port=3306\n");
    fprintf(file, "timeout=30.5\n");
    fclose(file);

    // 初始化
    config_error_t result = config_init("test_config.ini");
    assert(result == CONFIG_SUCCESS);

    // 测试INI文件读取
    const char* name = config_get_string("name", "unknown");
    assert(strcmp(name, "myapp") == 0);

    const char* version = config_get_string("version", "0.0");
    assert(strcmp(version, "1.0") == 0);

    bool debug = config_get_bool("debug", false);
    assert(debug == true);

    int port = config_get_int("port", 0);
    assert(port == 3306);

    double timeout = config_get_double("timeout", 0.0);
    assert(timeout > 30.4 && timeout < 30.6);

    // 清理测试文件
    remove("test_config.ini");

    printf("INI file test passed!\n");
}

void test_env_override() {
    printf("Testing environment variable override...\n");

    // 创建INI文件
    FILE* file = fopen("test_config.ini", "w");
    fprintf(file, "test_key = ini_value\n");
    fclose(file);

    // 设置环境变量
    setenv("TEST_KEY", "env_value", 1);

    // 初始化
    config_error_t result = config_init("test_config.ini");
    assert(result == CONFIG_SUCCESS);

    // 环境变量应该覆盖INI文件
    const char* value = config_get_string("TEST_KEY", "default");
    assert(strcmp(value, "env_value") == 0);

    // 清理
    remove("test_config.ini");

    printf("Environment override test passed!\n");
}

void test_error_handling() {
    printf("Testing error handling...\n");

    // 测试不存在的文件
    config_error_t result = config_init("non_existent_file.ini");
    assert(result == CONFIG_ERROR_FILE_NOT_FOUND);

    // 测试无效的INI文件
    FILE* file = fopen("invalid.ini", "w");
    fprintf(file, "[invalid section\n");  // 缺少结束括号
    fclose(file);

    result = config_init("invalid.ini");
    assert(result == CONFIG_ERROR_PARSE_ERROR);

    remove("invalid.ini");

    printf("Error handling test passed!\n");
}

int main() {
    printf("Running fread4c tests...\n\n");

    test_env_variables();
    config_cleanup();

    test_ini_file();
    config_cleanup();

    test_env_override();
    config_cleanup();

    test_error_handling();
    config_cleanup();

    printf("\nAll tests passed! ✓\n");
    return 0;
}
