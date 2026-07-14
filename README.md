# SlimCommonLog
<a href="https://codeberg.org/greergan/SlimTS">
  <img src="https://raw.githubusercontent.com/greergan/SlimTS/master/assets/slimts_logo.png" width="75" alt="SlimTS Logo">
</a>
A lightweight, thread-safe C++ logging library with ANSI color output and runtime-configurable log filtering.
Part of the [SlimCommon](https://codeberg.org/greergan/SlimCommon) library.  
Built using [SlimLibraryPackager](https://codeberg.org/greergan/SlimLibraryPackager).  
CI/CD supplied by unified workflows provided by [SlimLibraryPackager](https://codeberg.org/greergan/SlimLibraryPackager).

## Table of Contents
- [Features](#features)
- [API Reference](#api-reference)
- [Output Format](#output-format)
- [Building](#building)
- [Dependencies](#dependencies)
- [Examples](#examples)

## Features
- Four log levels: `TRACE`, `DEBUG`, `ERROR`, and `INFO`
- ANSI color-coded terminal output
- Thread-safe via per-stream mutexes
- Runtime-configurable filtering via a custom predicate (`set_can_log`)
- Structured `Message` type carrying file, function, line, and consumer context

[↑ Top](#table-of-contents)

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
void info(std::string_view text);
void set_can_log(std::function<bool(
    std::string_view consumer,
    std::string_view log_level,
    std::string_view file,
    std::string_view function
)> predicate);
}
```
[↑ Top](#table-of-contents)

## Output Format
```
LEVEL=>          <line> | <file> | <function> | <text>
```
`TRACE`, `DEBUG`, and `ERROR` emit structured output with label, line number, file, function, and message text. `INFO` emits plain text only. `ERROR` writes to `stderr`; all other levels write to `stdout`.

Each field is colored independently:

<table>
<tr><th>Element</th><th>Color</th></tr>
<tr><td><code>TRACE</code></td><td>${\color{cyan}Cyan}$</td></tr>
<tr><td><code>DEBUG</code></td><td>${\color{yellow}Yellow}$</td></tr>
<tr><td><code>ERROR</code></td><td>${\color{red}Red}$</td></tr>
<tr><td>File / Line</td><td>${\color{magenta}Magenta}$</td></tr>
<tr><td>Function</td><td>${\color{green}Green}$</td></tr>
<tr><td>Text / INFO</td><td>${\color{white}White}$</td></tr>
</table>

[↑ Top](#table-of-contents)

## Building

This library is built using [SlimLibraryPackager](https://codeberg.org/greergan/SlimLibraryPackager). See that repository for build instructions.

[↑ Top](#table-of-contents)


## Dependencies
```
none
```

[↑ Top](#table-of-contents)

## Examples
```cpp
// Basic logging
#include <slim/common/log.h>
log::trace(log::Message(__func__, "entering function", __FILE__, __LINE__));
log::debug(log::Message(__func__, "some debug info",   __FILE__, __LINE__));
log::error(log::Message(__func__, "something failed",  __FILE__, __LINE__));
log::info("plain text, no metadata");
```
```cpp
// Runtime filtering: allow only trace and debug
log::set_can_log([](
    std::string_view consumer,
    std::string_view log_level,
    std::string_view file,
    std::string_view function) -> bool
{
    return log_level == "trace" || log_level == "debug";
});
```
```cpp
// Consumer tagging for per-subsystem filtering
log::Message(__func__, "text", __FILE__, __LINE__, "my_subsystem");
```
```cpp
// Custom field separator
log::Message(__func__, "text", __FILE__, __LINE__, "my_subsystem", " :: ");
```
[↑ Top](#table-of-contents)
