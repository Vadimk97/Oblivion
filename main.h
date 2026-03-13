#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPS 1e-12

double** allocate_mat(int n) {
    double **m = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++)
        m[i] = (double*)malloc(n * sizeof(double));
    return m;
}

void free_mat(double **m, int n) {
    for (int i = 0; i < n; i++)
        free(m[i]);
    free(m);
}

void input_mat(double **m, int n) {
    printf("%d %d", n, n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            scanf("%lf", &m[i][j]);
}

double det_gauss(double **m, int n) {
    double **a = allocate_mat(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[i][j] = m[i][j];

    double det = 1.0;
    int sign = 1;

    for (int i = 0; i < n; i++) {
        int max_row = i;
        double max_val = fabs(a[i][i]);
        for (int k = i+1; k < n; k++) {
            if (fabs(a[k][i]) > max_val) {
                max_val = fabs(a[k][i]);
                max_row = k;
            }
        }
        if (max_val < EPS) {
            free_mat(a, n);
            return 0.0;
        }
        if (max_row != i) {
            double *tmp = a[i];
            a[i] = a[max_row];
            a[max_row] = tmp;
            sign = -sign;
        }
        double x = a[i][i];
        det *= x;

        for (int k = i+1; k < n; k++) {
            double factor = a[k][i] / x;
            for (int j = i; j < n; j++) {
                a[k][j] -= factor * a[i][j];
            }
        }
    }

    free_mat(a, n);
    return sign * det;
}

int main() {
    int n;
    scanf("%d", &n);
    if (n <= 0) {
        return 1;
    }

    double **matrix = allocate_mat(n);
    input_mat(matrix, n);

    double det = det_gauss(matrix, n);
    printf("%lf\n", det);

    free_mat(matrix, n);
    return 0;
}
