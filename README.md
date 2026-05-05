# slim::common::log

A lightweight, thread-safe C++ logging library with ANSI color output and runtime-configurable log filtering.

## Features

- Four log levels: `TRACE`, `DEBUG`, `ERROR`, and `INFO`
- ANSI color-coded terminal output
- Thread-safe via per-stream mutexes
- Runtime-configurable filtering via a custom predicate (`set_can_log`)
- Structured `Message` type carrying file, function, line, and consumer context

## Usage

### Basic Logging

```cpp
#include <slim/common/log.h>

log::trace(log::Message(__func__, "entering function", __FILE__, __LINE__));
log::debug(log::Message(__func__, "some debug info",   __FILE__, __LINE__));
log::error(log::Message(__func__, "something failed",  __FILE__, __LINE__));
log::info("plain text, no metadata");
```

`TRACE`, `DEBUG`, and `ERROR` emit structured output with label, line number, file, function, and message text. `INFO` emits plain text only.

`ERROR` writes to `stderr`; all other levels write to `stdout`.

### Filtering with `set_can_log`

By default all messages are allowed through. Supply a predicate to filter at runtime:

```cpp
log::set_can_log([](
    std::string_view consumer,
    std::string_view log_level,
    std::string_view file,
    std::string_view function) -> bool
{
    return log_level == "trace" || log_level == "debug";
});
```

The predicate receives four string views and returns `true` to allow a message or `false` to suppress it. `INFO` bypasses the predicate and always prints.

### Consumer Tagging

The consumer defaults to `"console"`. Pass a custom name to enable per-subsystem filtering:

```cpp
log::Message(__func__, "text", __FILE__, __LINE__, "my_subsystem");
```

The consumer is forwarded to `can_log` so your predicate can route or suppress messages by subsystem.

### Field Separator

The field separator defaults to `"|"` but can be overridden as the final argument:

```cpp
log::Message(__func__, "text", __FILE__, __LINE__, "my_subsystem", " :: ");
```

## API Reference

```cpp
namespace slim::common::log {

struct Message {
    Message(
        std::string_view function,
        std::string_view text,
        std::string_view file,
        int              line,
        std::string_view consumer  = "console",
        std::string_view separator = "|"
    );
};

void trace(Message message);
void debug(Message message);
void error(Message message);
void info(std::string_view text);   // no metadata, always prints

void set_can_log(std::function<bool(
    std::string_view consumer,
    std::string_view log_level,
    std::string_view file,
    std::string_view function
)> predicate);

} // namespace slim::common::log
```

## Output Format

```
LEVEL=>          <line> | <file> | <function> | <text>
```

Each field is colored independently:

<table>
<tr><th>Element</th><th>Color</th></tr>
<tr><td><code>TRACE</code></td><td style="color:#00cdcd;font-weight:bold">Cyan</td></tr>
<tr><td><code>DEBUG</code></td><td style="color:#cdcd00;font-weight:bold">Yellow</td></tr>
<tr><td><code>ERROR</code></td><td style="color:#cd0000;font-weight:bold">Red</td></tr>
<tr><td>File / Line</td><td style="color:#cd00cd;font-weight:bold">Magenta</td></tr>
<tr><td>Function</td><td style="color:#00cd00;font-weight:bold">Green</td></tr>
<tr><td>Text / INFO</td><td style="color:#e5e5e5;font-weight:bold">White</td></tr>
</table>

## Building

Include `slim/common/log.h` and compile `src/main.cpp` alongside your project. No external dependencies beyond a C++17-capable compiler.

```cmake
target_sources(your_target PRIVATE src/main.cpp)
target_include_directories(your_target PRIVATE include)
```
