# CPools

Fast and easy C89 thread pool library, using a [cross-platform threading library](https://github.com/PerformanC/CThreads).

## Features

- Cross-platform
- No dynamic memory allocation
- C89 compatible

> [!NOTE]
> This library is experimental, may have bugs and may not work as expected.

> [!WARNING]
> `pthread` or `Windows Threads` may use dynamic memory allocation underneath.

## Installation

### 1. Copy the files

Copy both cpools.h and cthreads.h to your project:

```bash
$ mv cpools.h /path/to/project/include/
$ mv cthreads.h /path/to/project/include/
```

### 2. Include the files

To ensure your compiler can find the files, you must add the include directory to your compiler's include path.

```bash
$ gcc -I/path/to/project/include/ ...
```

> [!NOTE]
> The flag may vary depending on your compiler.

## Usage

An example of how to use CPools can be found in the [tests](tests) directory. Showing all the features of CPools.

## Documentation

We still don't have a documentation for CPools, but we are working on it.

## Support

Any question or issue related to CPools or other PerformanC projects can be can be made in [PerformanC's Discord server](https://discord.gg/uPveNfTuCJ).

For verified issues, please also create a GitHub issue for tracking the issue.

## Contributing

CPools follows the PerformanC's [contribution guidelines](https://github.com/PerformanC/contributing). It is necessary to follow the guidelines to contribute to CPools and other PerformanC projects.

## Projects using CPools

None yet

## License

CPools is licensed under PerformanC's License, which is a modified version of the MIT License, focusing on the protection of the source code and the rights of the PerformanC team over the source code.