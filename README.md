# 💎 Vertices SDK

## 📑 Specifications

The Vertices SDK provides developers with an easy way to implement Blockchain wallets on embedded devices.

We are doing our best to achieve those goals:

- C library, can be included into C++ projects.
- Can be easily imported into any project
- Examples that will be provided:
  - [Unix-based OSes](examples/unix)
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

> ⚠️ IMPORTANT
> - The current version of the SDK is still *very early*;
> - It is not advised to use it in a production environment;
> - Use it at your own risk.

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
├── mdk                 # the SDK is running on several architecture, those files provide some compiler abstraction
│   └── compilers.h
└── utils               # tools to make things easier, clearer, smarter :) 
    └── utils.cmake
```

## 🧰 Installation

This repository is intended to be used as an external component to your project such as a submodule.

```shell
# clone into a directory
git clone <url> [path]
# clone into a directory including its submodules
git clone --recurse-submodules <url> [path]
# clone as submodule into an optionally specified location
git submodule add <url> [path]
```

### Environments

Using Conda is a great way to isolate the development environment ([more about Conda](https://interrupt.memfault.com/blog/conda-developer-environments)). 

We are providing an [environment file](utils/environment.yml) to be installed:

```shell
conda env create -f utils/environment.yml
conda activate vertices
```

The Conda environment will be updated accordingly to the source code if a new dependency or version is needed. 
Once you have checked-out the repo at a different commit, make sure to update your environment to use the right dependencies:

```shell
conda env update -f utils/environment.yml
```

🔜 We might also provide a Docker container once we have set up CI/CD.


### Configuration

A config file provides an easy way to configure the SDK: [`inc/vertices_config.h`](inc/vertices_config.h). The file is fully documented.

### Compilation

CMake is currently used to build the library and examples (GNU Make is on the roadmap).

#### CMake

In order to build the source using CMake in a separate directory (recommended), just enter at the command line:

```shell
mkdir build && cd build
cmake ..

# build static library: lib/libvertices.a
make vertices

# build Unix example
make unix_example
```

#### Make

👎 Soon.

## 🚀 Getting started

👉 More to come about how to import the package into your build system.

> 💡 Vertices is providing [examples](examples/) with various major SDKs, such as the ESP-IDF for Espressif microcontrollers. You can probably copy-paste our source code into your project 🙂.

### Examples

The example running on Unix is ready to be tested, [checkout the Readme](examples/unix/README.md).

## 📐 Tests

Make sure you have `cpputest` installed:

- Linux - `sudo apt-get install cpputest lcov`
- OSX - `brew install cpputest lcov`

From [`/tests`](/tests) you will be able to launch the unique command to run all the tests:

```shell
make
```

Checkout the [Readme](/tests/README.md) for more information.

## 🙌 Contributing

🤗 We gratefully accept bug reports and contributions from the community.

1.  Check for open issues or [start a discussion](https://discord.com/invite/2bTuWg5gGE) around a feature idea or a bug.
2.  Fork the repository on GitHub to start making your changes. As a general rule, you should use the "development" branch as a basis.
3.  Write a test which shows that the bug was fixed or that the feature works as expected.
4.  Send a pull request and bug us until it gets merged and published. Contributions may need some modifications, so work with us to get your change accepted!

