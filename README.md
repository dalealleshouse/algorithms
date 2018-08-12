# Algorithms
Examples of popular algorithms in several different languages. The purpose is to
demonstrate algorithm design and analysis fundamentals. A few things to note:

The project in general
- This project is part of a book I'm planning to write. I'd be happy to accept
    any pull requests. Obviously, contributors will be recognized in the book.
- The build and test environments are captured in docker containers. The
    examples demonstrate how to use the containers as stored on [Docker
    Hub](https://hub.docker.com). However, the [docker](docker/) folder has all
    files required to build the containers locally if that is the preference.

Implementation Quality
- Several of the implementations utilize recursion. Recursion provides an
    elegant means of demonstrating algorithms; however, it causes stack overflow
    exceptions when the input is sufficiently large (in non-tail call optimized
    languages). For many purposes, this is not an issue; however, use in
    production scenarios should be thoroughly scrutinized.
- Every attempt was made to ensure the accuracy of the implementations. For
    instance, each has associated unit tests. The C code is subjected to several
    clang sanitizers as well as clang tidy. However, this is not production
    hardened code.

Actual run time data
- Actual run time data for the C implementations is included. C was chosen
    as the language for showcasing run times because it executes "closest to the
    metal" and therefore provides the most accurate representation of an
    algorithm's run time. However, even though the actual run time will be
    different, the comparative difference between the run times should be
    commiserate in your language of choice.
- Keep in mind that the actual run-time is of little importance because it
    applies only to the machine on which the test was run. The intended purpose
    of providing the run time data is for comparison of run times between
    algorithms.
- All run time data was collected using a docker container running on a Surface
    Book 2 Laptop (Intel Core i7, 16GB RAM). Each time value represents the
    median of 3 separate executions.
- You can recreate the data using your own
    hardware by running the time_charts.sh bash script located in each c
    directory.

## Index
* [Sorting](sorting/)
..-[C](sorting/c/)
..-[Python](sorting/python/)
* [Quick Select](quick_select/)
..-[C](quick_select/c/)

## Build/Tests
### C
All units tests are written with
[cunit](http://cunit.sourceforge.net/doc/index.html). To run the tests,
navigate to the folder containing the C code and run the following command:

``` bash
docker run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c
```
### Python
All units tests are written with [pytest](https://docs.pytest.org/en/latest/).
To run the tests, navigate to the folder containing the python code and run the
following command:

``` bash
docker run --rm -v $(pwd):/src/ dalealleshouse/algo_test_runner_py
```
