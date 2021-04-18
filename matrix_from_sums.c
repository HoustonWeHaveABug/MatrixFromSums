#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include "mtrand.h"

typedef struct {
	double weight;
	int weight_int;
}
cell_t;

typedef struct {
	int x;
	int sums_x_delta;
	double weight_delta;
	cell_t *cell;
}
correction_t;

void set_cell_and_weight_sums(cell_t *, int, int);
void set_correction(correction_t *, int, int, cell_t *);
int compare_corrections(const void *, const void *);
void apply_correction(correction_t *);
void print_matrix(const char *);
int rand_int(int);
void free_data(void);

int rows_n, columns_n, *sums_y, *sums_x, total_sums_y;
cell_t *cells;
correction_t *corrections;

int main(void) {
	int val_max, cells_n, y, x, cell_idx;
	if (scanf("%d%d%d", &rows_n, &columns_n, &val_max) != 3 || rows_n < 1 || columns_n < 1 || val_max < 0 || rows_n < columns_n || rows_n > INT_MAX/columns_n) {
		fprintf(stderr, "Parameters expected on standard input\n");
		fprintf(stderr, "- rows_n (number of rows) > 0\n");
		fprintf(stderr, "- columns_n (number of columns) > 0\n");
		fprintf(stderr, "- val_max (maximum value) >= 0\n");
		fprintf(stderr, "With rows_n >= columns_n and rows_n*columns_n <= INT_MAX\n");
		fprintf(stderr, "If val_max is > 0 then row/column sums are generated from random values between 1 and val_max\n");
		fprintf(stderr, "Otherwise row/column sums are read from standard input\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	sums_y = calloc((size_t)(rows_n*2), sizeof(int));
	if (!sums_y) {
		fprintf(stderr, "Could not allocate memory for sums_y\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	sums_x = calloc((size_t)(columns_n*2), sizeof(int));
	if (!sums_x) {
		fprintf(stderr, "Could not allocate memory for sums_x\n");
		fflush(stderr);
		free(sums_y);
		return EXIT_FAILURE;
	}
	cells_n = rows_n*columns_n;
	cells = malloc(sizeof(cell_t)*(size_t)cells_n);
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		fflush(stderr);
		free(sums_x);
		free(sums_y);
		return EXIT_FAILURE;
	}
	corrections = malloc(sizeof(correction_t)*(size_t)columns_n);
	if (!corrections) {
		fprintf(stderr, "Could not allocate memory for corrections\n");
		fflush(stderr);
		free(cells);
		free(sums_x);
		free(sums_y);
		return EXIT_FAILURE;
	}
	smtrand((unsigned long)time(NULL));
	total_sums_y = 0;
	if (val_max > 0) {
		for (y = 0; y < rows_n; y++) {
			for (x = 0; x < columns_n; x++) {
				int val = rand_int(val_max)+1;
				sums_y[y] += val;
				sums_x[x] += val;
			}
			if (sums_y[y] > INT_MAX-total_sums_y) {
				fprintf(stderr, "Total of sums overflow\n");
				fflush(stderr);
				free_data();
				return EXIT_FAILURE;
			}
			total_sums_y += sums_y[y];
		}
	}
	else {
		int total_sums_x;
		for (y = 0; y < rows_n; y++) {
			if (scanf("%d", sums_y+y) != 1 || sums_y[y] < columns_n || sums_y[y] > INT_MAX-total_sums_y) {
				fprintf(stderr, "Invalid row sum or total of row sums overflow\n");
				fflush(stderr);
				free_data();
				return EXIT_FAILURE;
			}
			total_sums_y += sums_y[y];
		}
		total_sums_x = 0;
		for (x = 0; x < columns_n; x++) {
			if (scanf("%d", sums_x+x) != 1 || sums_x[x] < rows_n || sums_x[x] > INT_MAX-total_sums_x) {
				fprintf(stderr, "Invalid column sum or total of column sums overflow\n");
				fflush(stderr);
				free_data();
				return EXIT_FAILURE;
			}
			total_sums_x += sums_x[x];
		}
		if (total_sums_y != total_sums_x) {
			fprintf(stderr, "Mismatch between total of row sums and total of column sums\n");
			fflush(stderr);
			free_data();
			return EXIT_FAILURE;
		}
	}
	puts("SUMS Y");
	for (y = 0; y < rows_n; y++) {
		printf("%d\n", sums_y[y]);
	}
	puts("SUMS X");
	for (x = 0; x < columns_n; x++) {
		printf("%d\n", sums_x[x]);
	}
	fflush(stdout);
	for (y = 0; y < rows_n; y++) {
		for (x = 0; x < columns_n; x++) {
			set_cell_and_weight_sums(cells+y*columns_n+x, y, x);
		}
	}
	print_matrix("ORIGINAL");
	for (y = 0; y < rows_n; y++) {
		int sums_y_delta = sums_y[y]-sums_y[y+rows_n];
		if (sums_y_delta > 0) {
			int corrections_n = 0;
			for (x = 0; x < columns_n; x++) {
				int sums_x_delta = sums_x[x]-sums_x[x+columns_n];
				if (sums_x_delta > 0) {
					set_correction(corrections+corrections_n, x+columns_n, sums_x_delta, cells+y*columns_n+x);
					corrections_n++;
				}
			}
			qsort(corrections, (size_t)corrections_n, sizeof(correction_t), compare_corrections);
			while (sums_y_delta > 0) {
				sums_y[y+rows_n]++;
				corrections_n--;
				apply_correction(corrections+corrections_n);
				sums_y_delta--;
			}
		}
	}
	print_matrix("AFTER CORRECTIONS");
	for (cell_idx = 0; cell_idx < cells_n; cell_idx++) {
		int rand_y1 = rand_int(rows_n), rand_x1 = rand_int(columns_n), rand_y2 = rand_int(rows_n), rand_x2 = rand_int(columns_n);
		if (rand_y1 != rand_y2 && rand_x1 != rand_x2) {
			int cell_dec1 = rand_y1*columns_n+rand_x1, cell_dec2 = rand_y2*columns_n+rand_x2, offset_max;
			if (cells[cell_dec1].weight_int < cells[cell_dec2].weight_int) {
				offset_max = cells[cell_dec1].weight_int;
			}
			else {
				offset_max = cells[cell_dec2].weight_int;
			}
			if (offset_max > 1) {
				int offset = rand_int(offset_max);
				cells[cell_dec1].weight_int -= offset;
				cells[cell_dec2].weight_int -= offset;
				cells[rand_y1*columns_n+rand_x2].weight_int += offset;
				cells[rand_y2*columns_n+rand_x1].weight_int += offset;
			}
		}
	}
	print_matrix("AFTER RANDOM CHANGES");
	free_data();
	return EXIT_SUCCESS;
}

void set_cell_and_weight_sums(cell_t *cell, int y, int x) {
	cell->weight = (double)sums_y[y]*sums_x[x]/total_sums_y;
	cell->weight_int = (int)floor(cell->weight);
	sums_y[y+rows_n] += cell->weight_int;
	sums_x[x+columns_n] += cell->weight_int;
}

void set_correction(correction_t *correction, int x, int sums_x_delta, cell_t *cell) {
	correction->x = x;
	correction->sums_x_delta = sums_x_delta;
	correction->weight_delta = cell->weight-cell->weight_int;
	correction->cell = cell;
}

int compare_corrections(const void *a, const void *b) {
	const correction_t *correction_a = (const correction_t *)a, *correction_b = (const correction_t *)b;
	if (correction_a->sums_x_delta != correction_b->sums_x_delta) {
		return correction_a->sums_x_delta-correction_b->sums_x_delta;
	}
	if (correction_a->weight_delta < correction_b->weight_delta) {
		return -1;
	}
	if (correction_a->weight_delta > correction_b->weight_delta) {
		return 1;
	}
	if (correction_a->cell < correction_b->cell) {
		return -1;
	}
	return 1;
}

void apply_correction(correction_t *correction) {
	sums_x[correction->x]++;
	correction->cell->weight_int++;
}

void print_matrix(const char *title) {
	int y, x;
	printf("WEIGHTS - %s\n", title);
	for (y = 0; y < rows_n; y++) {
		printf("%d", cells[y*columns_n].weight_int);
		for (x = 1; x < columns_n; x++) {
			printf(" %d", cells[y*columns_n+x].weight_int);
		}
		puts("");
	}
	printf("WEIGHT SUMS Y - %s\n", title);
	for (y = 0; y < rows_n; y++) {
		printf("%d (delta %d)\n", sums_y[y+rows_n], sums_y[y]-sums_y[y+rows_n]);
	}
	printf("WEIGHT SUMS X - %s\n", title);
	for (x = 0; x < columns_n; x++) {
		printf("%d (delta %d)\n", sums_x[x+columns_n], sums_x[x]-sums_x[x+columns_n]);
	}
	fflush(stdout);
}

int rand_int(int val) {
	return (int)emtrand((unsigned long)val);
}

void free_data(void) {
	free(corrections);
	free(cells);
	free(sums_x);
	free(sums_y);
}
