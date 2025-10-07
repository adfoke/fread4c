# fread4c

A lightweight C library for reading configuration files with environment variable override support.

## Features

- **Environment Variable Priority**: Environment variables take precedence over config file values
- **INI File Support**: Standard INI format with sections and comments
- **Multiple Data Types**: Support for strings, integers, booleans, and floating-point numbers
- **Default Values**: Automatic fallback to default values when keys are not found
- **Error Handling**: Comprehensive error reporting
- **Cross-platform**: Uses standard C libraries, no external dependencies
- **Thread-safe**: Single initialization pattern

## Installation

### Build from Source

```bash
# Clone the repository
git clone https://github.com/adfoke/fread4c.git
cd fread4c

# Build the library and tests
make

# Run tests
make test

# Install (optional)
sudo make install
```

### Integration

Include the header file in your project:
```c
#include "config.h"
```

Link with the static library:
```bash
gcc your_program.c -L/path/to/fread4c -lfread4c -o your_program
```

## API Reference

### Initialization

```c
config_error_t config_init(const char* filename);
```
Initialize the configuration system. Pass `NULL` to only use environment variables, or a filename to load an INI config file.

### Getting Values

```c
const char* config_get_string(const char* key, const char* default_value);
int config_get_int(const char* key, int default_value);
bool config_get_bool(const char* key, bool default_value);
double config_get_double(const char* key, double default_value);
```

Retrieve configuration values. Environment variables are checked first, then the config file.

### Cleanup

```c
void config_cleanup();
```
Free all allocated resources. Call this when done using the library.

### Error Handling

```c
const char* config_get_error_message();
```
Get the last error message.

## Configuration Format

### INI File Format

```ini
# Comments start with # or ;
[section_name]
key1 = value1
key2 = value2

[another_section]
key3 = value3
```

### Environment Variables

Environment variables are checked with case-insensitive matching:
```bash
export MY_CONFIG_KEY=value
export my_config_key=value  # Also works
```

## Usage Example

```c
#include <stdio.h>
#include "config.h"

int main() {
    // Initialize with config file
    if (config_init("app.ini") != CONFIG_SUCCESS) {
        printf("Error: %s\n", config_get_error_message());
        return 1;
    }

    // Get configuration values
    const char* app_name = config_get_string("name", "MyApp");
    int port = config_get_int("port", 8080);
    bool debug = config_get_bool("debug", false);
    double timeout = config_get_double("timeout", 30.0);

    printf("App: %s\n", app_name);
    printf("Port: %d\n", port);
    printf("Debug: %s\n", debug ? "true" : "false");
    printf("Timeout: %.2f\n", timeout);

    // Cleanup
    config_cleanup();
    return 0;
}
```

### Sample Config File (app.ini)

```ini
[app]
name = MyApplication
version = 1.0.0
debug = false

[server]
host = localhost
port = 8080
timeout = 30.5
ssl_enabled = true
```

### Environment Override

```bash
# Override config file values
export DEBUG=true
export PORT=9000
./your_program
```

## Error Codes

- `CONFIG_SUCCESS`: Operation successful
- `CONFIG_ERROR_FILE_NOT_FOUND`: Config file not found
- `CONFIG_ERROR_PARSE_ERROR`: Invalid config file format
- `CONFIG_ERROR_MEMORY_ERROR`: Memory allocation failed
- `CONFIG_ERROR_KEY_NOT_FOUND`: Configuration key not found

## Boolean Values

The following strings are recognized as boolean values (case-insensitive):
- **True**: `true`, `1`, `yes`, `on`
- **False**: `false`, `0`, `no`, `off`

## Thread Safety

The library uses a single initialization pattern and is thread-safe for read operations after initialization. However, `config_init()` and `config_cleanup()` should not be called concurrently.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
