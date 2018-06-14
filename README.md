# Algorithms
Examples of popular algorithms in several different languages.

All the algorithms have unit tests and an associated test runner docker
container.

## C
All units tests are written with
[cunit](http://cunit.sourceforge.net/doc/index.html). To run the tests,
navigate to the folder containing the C code and run the following command:

``` bash
docker run --rm -v $(pwd):/src dalealleshouse/algo_test_runner_c
```

The command above will download the docker container from docker hub. You can
build the container locally with the Docker file located int
[docker/c](docker/c).

## Python
All units tests are written with [pytest](https://docs.pytest.org/en/latest/).
To run the tests, navigate to the folder containing the python code and run the
following command:

``` bash
docker run --rm -v $(pwd):/src/ dalealleshouse/algo_test_runner_py
```

The command above will download the docker container from docker hub. You can
build the container locally with the Docker file located int
[docker/python](docker/python).

