# Wasmjit

Wasmjit is a small embeddable WebAssembly runtime. Its core is written in
C90 and is easily portable to most environments.

It includes an Emscripten-compatible host environment for running on
POSIX systems. This is useful for running WebAssembly modules without
having to run an entire browser or JavaScript runtime.

Wasmjit is licensed under a permissive MIT license.

# Getting Started

Wasmjit currently runs on x86_64 POSIX systems, like Linux, macOS, and
the BSDs. To get started with Wasmjit you need a few tools:

* A standard POSIX C development environment that includes `cc` and
  `make`.
* [Emscripten SDK](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html)

## Building the Executable

    $ make clean
    $ make -j wasmjit

## Building and running the Example

    $ source path_to_emsdk/emsdk_env.sh # bring emcc into $PATH
    $ emcc -o selfpipe.js src/wasmjit_examples/selfpipe.c

You can now run the example:

    $ time ./wasmjit selfpipe.wasm

# Status

Wasmjit can run a fairly large subset of Emscripten-generated WebAssembly
and has been tested on Linux, OpenBSD, and macOS. It
currently only supports x86_64. Here are the current developments goals
in order of priority:

* Implement an interpreter
* Implement Go-runtime for Go-generated wasm files
* Optimize x86_64 JIT
* Implement arm64 JIT
* Implement JIT debugging symbols

PRs are welcome :) see [CONTRIBUTING.md](./CONTRIBUTING.md) first.

# Contact

Rian Hunter [@cejetvole](https://twitter.com/cejetvole)
