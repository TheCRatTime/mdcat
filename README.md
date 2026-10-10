# mdcat
**mdcat** - Markdown console parser.

## Description
This is a render for Markdown markup in termial

### Features
- Fast render & minimal memory usage
- You can see output by `mdcat FILE`

### Build

Needed:
- C Compiler
- CMake 3.10 or greater
- GNU Make

```bash
cmake -B build
cd build/
make
```

### Authors
TheCRatTime

### License
Project is under Apache 2.0 license.

### See also
After build, see this readme with `mdcat`:

```bash
# build...
clear
./mdcat ../README.md
```
