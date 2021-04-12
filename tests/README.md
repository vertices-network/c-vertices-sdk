# Unit tests

## Setup

- Linux - `sudo apt-get install cpputest lcov`
- OSX - `brew install cpputest lcov`

## Running tests

```shell
make
```

## Directory structure

```
├── Makefile // Invokes all the unit tests
├── MakefileWorker.mk // Comes from CppUTest itself
├── MakefileWorkerOverrides.mk // Injected overrides
├── build
│   [...] // Where all the tests wind up
├── fakes
│   // fakes for unit tests
├── mocks
│   // mocks for unit tests
├── makefiles // Each c file you unit test has a makefile here
│   ├── Makefile_<module_name>.mk
│   └── [...]
├── src // test source files
└── test_*
```

# Adding a test

- Add a new test makefile under test/makefiles/. These just list the sources you
  will compile
- Add a new test file under tests/src for the module you want to test
- `make`

---

Based on the work of our friends at Memfault ([SDK](https://github.com/memfault/memfault-firmware-sdk)). For more, checkout the [Interrupt](https://interrupt.memfault.com/) blog.