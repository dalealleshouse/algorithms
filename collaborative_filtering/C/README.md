# Merge Sort
To run tests:
```
make
```

To build merge_sort object file
```
make merge_sort.o
```

## Requirements
- clang-6.0

    ```
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-add-repository "deb http://apt.llvm.org/xenial/
    llvm-toolchain-xenial-6.0 main"
    sudo apt-get update
    sudo apt-get install -y clang-6.0
    ```
- cunit

    ```
    sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev
    ```
