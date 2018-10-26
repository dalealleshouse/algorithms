[![Build Status](https://travis-ci.com/dalealleshouse/algorithms.svg?branch=master)](https://travis-ci.com/dalealleshouse/algorithms)

# Algorithms
Examples of popular algorithms in several different languages. The purpose is to
demonstrate algorithm design and analysis fundamentals. This project is part of
a book I'm planning to write. I'd be happy to accept any pull requests.
Contributors will be recognized in the book.

## A few things to note about..

Build environments
- The build and test environments are captured in docker containers. The
    examples demonstrate how to use the containers as stored on [Docker
    Hub](https://hub.docker.com). However, the [docker](docker/) folder has all
    files required to build the containers locally if that is the preference.

Implementation Quality
* Several of the implementations utilize recursion. Recursion provides an
    elegant means of demonstrating algorithms; however, it causes stack overflow
    exceptions when the input is sufficiently large (in non-tail call optimized
    languages). For many purposes, this is not an issue; however, use in
    production scenarios should be thoroughly scrutinized.
* Every attempt was made to ensure the quality of the implementations. For 
    instance, each has associated unit tests. The C code is subjected to several
    clang sanitizers as well as clang tidy. However, this is not production
    hardened code.
* The C code makes extensive use of the `rand()` function. For many
    reasons, this is not ideal. [Eternally
    Confused](http://www.eternallyconfuzzled.com/arts/jsw_art_rand.aspx) has a
    great article about what this is true. Furthermore, `rand()` is never
    seeded. This ensures that the actual run time comparisons between algorithms
    accepting randomly sorted arrays are using the exact same inputs.  For the
    purposes of this project, `rand()` it is sufficient. However, a different
    solution is required for production scenarios.
* The C code simply returns negative values for errors. This is no means of
    reporting error conditions.

Actual run time data
* Actual run time data for the C implementation of the algorithms is included 
    in each section. C was chosen as the language for showcasing run times
    because it executes "closest to the metal" and therefore provides the most
    accurate representation of an algorithm's run time. However, even though the
    actual run time wont be the same, the comparative difference between the run
    times should be commiserate in your language of choice.
* Milliseconds are close to non-consequential for most applications. Remember
    the words of the great Donald Knuth - "pre-mature optimization is the root
    of all evil."
* The actual run-time is of little importance because it applies only to the
    machine on which the test was run. The intended purpose of providing the run
    time data is for comparison between algorithms.
* All run time data was collected using a docker container running on a Surface
    Book 2 Laptop (Intel Core i7, 16GB RAM).
* Each time value represents the median of 3 separate executions.
* You can recreate the data using your own hardware by running the
    time_charts.sh bash script located in each c directory.

## Index

* [List Data Structures](list_data_structures/)
    * [Array](list_data_structures/README.md#arrays)
    * [Linked List](list_data_structures/README.md#linked-lists)
    * [Binary Tree](list_data_structures/README.md#binary-trees)
    * [Hash Tables](list_data_structures/README.md#hash-tables)
* [Sorting](sorting/)
    * [Bubble Sort](sorting/README.md#bubble-sort)
    * [Insertion Sort](sorting/README.md#insertion-sort)
    * [Selection Sort](sorting/README.md#selection-sort)
    * [Merge Sort](sorting/README.md#merge-sort)
    * [Quick Sort](sorting/README.md#quick-sort)
* [Quick Select](quick_select/)
* [Random Contraction](random_contraction/)

## Build/Tests
### C
All units tests are written with
[cunit](http://cunit.sourceforge.net/doc/index.html). To run the tests,
navigate to the folder containing the C code and run the following command:

``` bash
docker run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c
```

In order to pass the CI build, the C code is subjected to several clang
sanitizers which are configured in different build configurations. Use the
following command to run all configured builds.

``` bash
docker run --rm --privileged -v $(pwd):/src dalealleshouse/algo_test_runner_c ./validate.sh
```

To generate a code coverage report, use the following command. This will
generate a subdirectory named output with a full html report.

``` bash
docker run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c ./coverage.sh
```

To validate the C code for all algorithms, navigate to the root of the project
and run the following command.

``` bash
./ci_validate.sh
```

### Python
All units tests are written with [pytest](https://docs.pytest.org/en/latest/).
To run the tests, navigate to the folder containing the python code and run the
following command:

``` bash
docker run --rm -v $(pwd):/src/ dalealleshouse/algo_test_runner_py
```
