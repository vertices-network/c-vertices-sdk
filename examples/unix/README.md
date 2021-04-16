# Unix example

To make things easy to get started, we are providing an example running on your machine! 

## Welcome

We decided to make some parts external to the Vertices SDK but still necessary. Some are too close
to the metal and some benefit from a specific implementation: signing using a software library when 
there is a secure element available doesn't make sense.

Here are the custom parts that needs to be reimplemented for each target:
- the HTTP stack
- a random number generator
- Ed25519 signing

Obviously, those elements are provided in the example. In order to have the example run, you'll need to install 
`libcurl` and `libsodium`. You will also need to make sure the header files are accessible when building the example.

## Installation

If you are using the Conda [environment](../../utils/environment.yml), you can skip that step and continue to [compile](#compile).

#### macOS

```shell
brew install libsodium curl
```
  
On macOS, I would suggest to get the path of the brew-installed libraries. 
`brew --prefix libsodium` gives you the full path to `libsodium`, append `lib`
and `include` to it to find the path to the library and headers respectively. 
In my case, all the libraries and headers are located into `/opt/homebrew/lib/` and `/opt/homebrew/include/`.

#### Debians

```shell
apt-get install libsodium curl
```

## Compile

First, build the example. Go to the root directory and follow those steps:

```shell
# create build directory
mkdir /path/to/build_dir && cd /path/to/build_dir
cmake /path/to/mbedtls_source

# build Unix example
# this will fetch all the dependencies and build them (such as the Vertices SDK!)
make unix_example
```

The compiled program is located at `/path/to/build_dir/examples/unix/unix_example`.

## Run

The first time you launch the example, you might want to generate a new account that will then be used. 
Use the flag `-n` to allow `unix_example` to generate a new account. You will then be asked into put money to the 
account in order to execute new transactions.

```shell
# we assume you did not move from the build directory
# run with option -n if you want to create a new account
./examples/unix/unix_example [-n]
```
