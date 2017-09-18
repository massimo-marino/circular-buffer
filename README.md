# circular-buffer
C++ implementation of a circular buffer

#### Requirements

The cmake files compiles with `-std=c++14` (but it would be fine also with `-std=c++1z`).
It does not compile with `-std=c++11` since `std::make_unique()` is used.

Cmake file uses `clang++-4.0` by default: install clang 4.0, or another release and in this case change the cmake file.

`g++` can be used uncommenting its lines and commenting the clang lines in the cmake files.


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
