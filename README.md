# circular-buffer
C++17 implementation of a circular buffer

#### Requirements

`cmake 3.26.3` is used to compile the sources.

The cmake file compiles with `C++17`.

The unit tests are implemented in googletest: be sure you have installed googletest to compile.


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
