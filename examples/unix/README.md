# Unix example

To make things easy to get started, we are providing an example running on your machine! 

## Compiling

We decided to make some parts external to the Vertices SDK but still necessary. Some are too close
to the metal and some benefit from a specific implementation: signing using a software library when 
there is a secure element available doesn't make sense.

Here are the custom parts that needs to be reimplemented for each target:
- the HTTP stack
- a random number generator
- Ed25519 signing

In order to have the example run, you'll need to install `libcurl` and `libsodium`. You will also need to make sure 
the header files are accessible when building the example.

### macOS

```shell
brew install libsodium curl
```
  
On macOS, I would suggest to get the path of the brew-installed libraries. 
`brew --prefix libsodium` gives you the full path to `libsodium`, append `lib`
and `include` to it to find the path to the library and headers respectively. 
In my case, all the libraries and headers are located into `/opt/homebrew/lib/` and `/opt/homebrew/include/`.

### Debians

```shell
apt-get install libsodium curl
```


