# circular-buffer
C++17 implementation of a circular buffer

#### Requirements

`cmake` is used to compile the sources.

The default compiler used is `clang++-5.0`.

The cmake files compile with `-std=c++17`.

The unit tests are implemented in `googletest`: be sure you have installed `googletest` to compile.


#### Install

```bash
$ git clone https://github.com/massimo-marino/circular-buffer.git
$ cd circular-buffer
$ mkdir build
$ cd build
$ cmake ..
$ make
```

#### Run Unit Tests

```bash
$ cd src/unitTests
$ ./unitTests
```


#### Run Example

Remove `-DDO_LOGS` from `CMAKE_CXX_FLAGS` in the cmake file to see no logs printed at run-time.

```bash
$ cd ../example
$ ./circular-buffer-example
```
