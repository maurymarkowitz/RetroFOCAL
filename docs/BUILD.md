# Building RetroFOCAL

This file expands on the build instructions for RetroFOCAL. See the main README for high-level information and release installs.

## Dependencies

- `gcc` or `clang`
- `make`
- `flex`
- `bison`

### Unix-like (Linux/macOS)

1. Install tools via your package manager (e.g. `apt`, `dnf`, `brew`).
2. In project root, run:

```sh
make clean
make all
```

3. Optional install:

```sh
make install
```

4. Optional uninstall:

```sh
make uninstall
```

### Windows (MSYS2 / MinGW-W64)

1. Install MSYS2 from https://www.msys2.org/.
2. Open MINGW64 shell and install dependencies:

```sh
pacman -Syu
pacman -S --needed mingw-w64-x86_64-toolchain make flex bison
```

3. Build from project root:

```sh
make clean
make all CC=x86_64-w64-mingw32-gcc
```

4. Optional install (uses Windows `copy`/`robocopy` in makefile):

```sh
make install
```

5. Optional uninstall:

```sh
make uninstall
```

## Notes

- `PREFIX` may be overridden (e.g. `make PREFIX=/opt/install install`).
- `make install` on macOS / Linux defaults to `/usr/local`; on Windows defaults to `%PROGRAMFILES%\RetroFOCAL`.
