#include "./strassen.h"
#include <stdio.h>

bool ispowerof2(unsigned int x) { return x && !(x & (x - 1)); }

int add_matrices(const size_t n, const int x[n][n], const int y[n][n],
                 int output[n][n]) {
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      output[i][j] = x[i][j] + y[i][j];
    }
  }

  return 0;
}

void _add_matrices(const size_t n, const int x[n][n], const int y[n][n],
                   int output[n][n]) {
  int result = add_matrices(n, x, y, output);

  if (result != 0) {
    fprintf(stderr, "invalid result from add_matrices");
    exit(result);
  }
}

int subtract_matrices(const size_t n, const int x[n][n], const int y[n][n],
                      int output[n][n]) {
  for (size_t i = 0; i < n; i++)
    for (size_t j = 0; j < n; j++) output[i][j] = x[i][j] - y[i][j];

  return 0;
}

void _subtract_matrices(const size_t n, const int x[n][n], const int y[n][n],
                        int output[n][n]) {
  int result = subtract_matrices(n, x, y, output);
  if (result != 0) {
    fprintf(stderr, "invalid result from subtract_matrices");
    exit(result);
  }
}

int split(const size_t n, const int row_start, const int column_start,
          const int count, const int x[n][n], int output[count][count]) {
  for (int i = 0; i < count; i++) {
    for (int j = 0; j < count; j++) {
      output[i][j] = x[row_start + i][column_start + j];
    }
  }

  return 0;
}

void _split(const size_t n, const int row_start, const int column_start,
            const int count, const int x[n][n], int output[count][count]) {
  int result = split(n, row_start, column_start, count, x, output);

  if (result != 0) {
    fprintf(stderr, "invalid result from split");
    exit(result);
  }
}

int copy_quadrent(const size_t n, const int row_start, const int column_start,
                  const int count, const int x[count][count],
                  int output[n][n]) {
  for (int i = 0; i < count; i++) {
    for (int j = 0; j < count; j++) {
      output[row_start + i][column_start + j] = x[i][j];
    }
  }

  return 0;
}

void _copy_quadrent(const size_t n, const int row_start, const int column_start,
                    const int count, const int x[count][count],
                    int output[n][n]) {
  int result = copy_quadrent(n, row_start, column_start, count, x, output);

  if (result != 0) {
    fprintf(stderr, "invalid result from copy_quadrent");
    exit(result);
  }
}

int print_matrix(const char* name, const int n, const int x[n][n]) {
  printf("Matrix: %s\n", name);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      printf("%5d", x[i][j]);
    }
    puts("");
  }

  return 0;
}

int sub_matrices(const size_t n, const int source[n][n], int a[n / 2][n / 2],
                 int b[n / 2][n / 2], int c[n / 2][n / 2],
                 int d[n / 2][n / 2]) {
  int _n = n / 2;
  _split(n, 0, 0, _n, source, a);
  _split(n, 0, _n, _n, source, b);
  _split(n, _n, 0, _n, source, c);
  _split(n, _n, _n, _n, source, d);

  return 0;
}

void _sub_matrices(const size_t n, const int source[n][n], int a[n / 2][n / 2],
                   int b[n / 2][n / 2], int c[n / 2][n / 2],
                   int d[n / 2][n / 2]) {
  int result = sub_matrices(n, source, a, b, c, d);

  if (result != 0) {
    fprintf(stderr, "invalid result from sub_matrices");
    exit(result);
  }
}

void _multiply_square_matrices(const int n, const int x[n][n],
                               const int y[n][n], int output[n][n]) {
  int result = multiply_square_matrices(n, x, y, output);

  if (result != 0) {
    fprintf(stderr, "invalid result from multiply_square_matrices");
    exit(result);
  }
}

int multiply_square_matrices(const size_t n, const int x[n][n],
                             const int y[n][n], int output[n][n]) {
  if (x == NULL || y == NULL || output == NULL) return -1;

  if (!ispowerof2(n)) return -2;

  if (n == 1) {
    output[0][0] = x[0][0] * y[0][0];
    return 0;
  }

  int _n = n / 2;

  int a[_n][_n];
  int b[_n][_n];
  int c[_n][_n];
  int d[_n][_n];
  _sub_matrices(n, x, a, b, c, d);

  int e[_n][_n];
  int f[_n][_n];
  int g[_n][_n];
  int h[_n][_n];
  _sub_matrices(n, y, e, f, g, h);

  int temp[_n][_n];
  int temp2[_n][_n];

  int p1[_n][_n];
  _subtract_matrices(_n, f, h, temp);
  multiply_square_matrices(_n, a, temp, p1);

  int p2[_n][_n];
  _add_matrices(_n, a, b, temp);
  multiply_square_matrices(_n, temp, h, p2);

  int p3[_n][_n];
  _add_matrices(_n, c, d, temp);
  multiply_square_matrices(_n, temp, e, p3);

  int p4[_n][_n];
  _subtract_matrices(_n, g, e, temp);
  multiply_square_matrices(_n, d, temp, p4);

  int p5[_n][_n];
  _add_matrices(_n, a, d, temp);
  _add_matrices(_n, e, h, temp2);
  multiply_square_matrices(_n, temp, temp2, p5);

  int p6[_n][_n];
  _subtract_matrices(_n, b, d, temp);
  _add_matrices(_n, g, h, temp2);
  multiply_square_matrices(_n, temp, temp2, p6);

  int p7[_n][_n];
  _subtract_matrices(_n, a, c, temp);
  _add_matrices(_n, e, f, temp2);
  multiply_square_matrices(_n, temp, temp2, p7);

  // Quad 1
  _add_matrices(_n, p5, p4, temp);
  _subtract_matrices(_n, temp, p2, temp2);
  _add_matrices(_n, temp2, p6, temp);
  _copy_quadrent(n, 0, 0, _n, temp, output);

  // Quad 2
  _add_matrices(_n, p1, p2, temp);
  _copy_quadrent(n, 0, _n, _n, temp, output);

  // Quad 3
  _add_matrices(_n, p3, p4, temp);
  _copy_quadrent(n, _n, 0, _n, temp, output);

  // Quad 4
  _add_matrices(_n, p1, p5, temp);
  _subtract_matrices(_n, temp, p3, temp2);
  _subtract_matrices(_n, temp2, p7, temp);
  _copy_quadrent(n, _n, _n, _n, temp, output);

  return 0;
}

int brute_force(const size_t n, const int x[n][n], const int y[n][n],
                int output[n][n]) {
  if (x == NULL || y == NULL || output == NULL) return -1;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      output[i][j] = 0;
      for (size_t k = 0; k < n; k++) output[i][j] += x[i][k] * y[k][j];
    }
  }

  return 0;
}
