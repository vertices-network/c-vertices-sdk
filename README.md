# 💎 Vertices SDK

## Specifications

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

## Installation

This repository is intended to be used as an external component to your project such as a submodule.

```shell
# clone into a directory
git clone <url> [path]
# clone as submodule into an optionally specified location
git submodule add <url> [path]
```

### Configuration

A config file provides an easy way to configure the SDK: [`config/vertices_config.h`](config/vertices_config.h). The file is fully documented.

It is advised to copy that config file into your project if you want to track it with your Version Control System.

### Compilation

There are currently two build systems supported:

* CMake
* GNU Make

Those are providing the static library: `libvertices`.

#### CMake

In order to build the source using CMake in a separate directory (recommended), just enter at the command line:

```shell
mkdir /path/to/build_dir && cd /path/to/build_dir
cmake /path/to/mbedtls_source
make
```

#### Make

In order to build from the source code using GNU Make, just enter at the command line:

```shell
make
```

## Getting started

Although the Vertices SDK is providing simple ways to connect and interact with a blockchain, you will need to provide platform-specific functions.

* Functions to receive/send data through an HTTP socket:
    * `vertices_net_connect(char *url, int * socket_id)`
    * `vertices_net_send(int socket_id, char *data, size_t length)`
    * `vertices_net_receive(int socket_id, char *data, size_t *size)`
    * `vertices_net_close(int * socket_id)`

> 💡 Vertices will provide [examples](examples/) with various major SDKs, such as the ESP-IDF for Espressif microcontrollers. You can probably copy-paste our source code into your project 🙂.

## Tests

👎 There are no unit-tests at the moment.

## Contributing

🤗 We gratefully accept bug reports and contributions from the community.

1.  Check for open issues or [start a discussion](https://discord.gg/buaqNSum) around a feature idea or a bug.
2.  Fork the repository on GitHub to start making your changes. As a general rule, you should use the "development" branch as a basis.
3.  Write a test which shows that the bug was fixed or that the feature works as expected.
4.  Send a pull request and bug us until it gets merged and published. Contributions may need some modifications, so work with us to get your change accepted!

