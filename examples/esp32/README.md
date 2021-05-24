# ESP HTTP Client Example

## Adding the private and public keys

For the example to work, we need to add an account using the private and public keys.

The binary data is taken from a 64-byte binary file encapsulating both the private and public keys in binary format.
It has been generated using the [Unix example](../unix) available in the repo, using the `-n` flag.

```shell
./unix_example -n
```

You can then copy `private_key.bin` into the example's [`main`](main) directory and compile.

> ⚠️ This is not a safe way to handle keys.

## Compilation 

### ESP-IDF build system

For this example to run, you have to download and install [ESP-IDF](https://github.com/espressif/esp-idf) in a separate directory.

Make sure you have the `IDF_PATH` environment variable defined, either in your `~/.bashrc` or `~/.zshrc`, or you can run:

```shell
# from ESP-IDF directory
. export.sh
```

Once setup, go to the example directory (same location of the README you are reading) and run:

```shell
# 'flash' if you want to install the firmware on your target
# 'monitor' if you want to open the serial interface and see the logs
idf.py build flash monitor
```

### PlatformIO

Open the directory using PlatformIO and build.

