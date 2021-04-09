# 💎 Vertices SDK

## 📑 Specifications

The Vertices SDK provides developers with an easy way to implement Blockchain wallets on embedded devices.

We are doing our best to achieve those goals:

- C library, can be included into C++ projects.
- Can be easily imported into any project
- Examples that will be provided:
  - Unix-based OSes
  - ESP-IDF
  - Zephyr
- Connect to any Algorand API `algod` (local or remote provider)
- Build with CMake and Make
- Continuous Integration to maintain code quality:
  - Build static library for several targets
  - Unit-Testing

At Vertices Network, we want to free your mind from the hassle of a safe design. Here are other projects we are working on:

- Secure Element support for future and more secure designs
- Encrypted Flash for current designs

## 🧭 Repository structure

```shell
.
├── CMakeLists.txt      # root CMakeLists: use it if you want to try the examples
├── examples            # examples
│   ├── unix            # Unix example to run the wallet on your machine or Raspberry Pi...
│   └── ...             # more to come
├── external            # external libraries
│   ├── mpack           # we've got mpack for example
│   └── cJson           # cJSON to parse JSON payloads
├── inc                 # public include directory: you'll need those files to use the library
│   ├── vertices.h      # for example, `vertices.h`
│   └── ...
├── lib                 # 
│   ├── inc             # "private" header files, used within the library
│   ├── algorand        # implementation of Vertices with Algorand. The Algorand provider is the first implemented.
│   ├── http            # HTTP wrappers functions, GET, POST... several implementations available for several stacks (libcurl, ESP-IDF...)
│   ├── CMakeLists.txt  # CMake of the Vertices SDK, exports a package to be imported in your project, see examples' CMakeLists
│   └── ...             # source files
├── mdk                 # the SDK is running on several architecture, we need some compiler abstraction
│   └── compilers.h
└── utils               # tools to make things easier, clearer, smarter :) 
    └── utils.cmake
```

## 🧰 Installation

This repository is intended to be used as an external component to your project such as a submodule.

```shell
# clone into a directory
git clone <url> [path]
# clone as submodule into an optionally specified location
git submodule add <url> [path]
```

### Configuration

A config file provides an easy way to configure the SDK: [`config/vertices_config.h`](examples/unix/config/vertices_config.h). The file is fully documented.

It is advised to copy that config file into your project if you want to track it with your Version Control System.

### Compilation

There are currently two build systems supported:

* CMake
* GNU Make

Those are providing the static library: `libvertices.a`.

#### CMake

In order to build the source using CMake in a separate directory (recommended), just enter at the command line:

```shell
mkdir /path/to/build_dir && cd /path/to/build_dir
cmake /path/to/mbedtls_source
make
```

#### Make

👎 Soon.

## 🚀 Getting started

👉 More to come about how to import the package into your build system.

> 💡 Vertices will provide [examples](examples/) with various major SDKs, such as the ESP-IDF for Espressif microcontrollers. You can probably copy-paste our source code into your project 🙂.

### Examples

The example running on Unix is ready to be tested, [checkout the Readme](examples/unix/README.md).

## 📐 Tests

👎 There are no unit-tests at the moment.

## 🙌 Contributing

🤗 We gratefully accept bug reports and contributions from the community.

1.  Check for open issues or [start a discussion](https://discord.com/invite/2bTuWg5gGE) around a feature idea or a bug.
2.  Fork the repository on GitHub to start making your changes. As a general rule, you should use the "development" branch as a basis.
3.  Write a test which shows that the bug was fixed or that the feature works as expected.
4.  Send a pull request and bug us until it gets merged and published. Contributions may need some modifications, so work with us to get your change accepted!

