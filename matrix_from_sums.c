#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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

int rows_n, columns_n, *sums_y, *sums_x, total;
cell_t *cells;

int main(void) {
	int val_max, cells_n, y, x, cell_idx;
	correction_t *corrections;
	if (scanf("%d%d%d", &rows_n, &columns_n, &val_max) != 3 || rows_n < 1 || columns_n < 1 || val_max < 1 || rows_n < columns_n) {
		fprintf(stderr, "Parameters expected on standard input\n");
		fprintf(stderr, "- number of rows > 0\n");
		fprintf(stderr, "- number of columns > 0\n");
		fprintf(stderr, "- maximum value > 0\n");
		fprintf(stderr, "With number of rows >= number of columns\n");
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
	total = 0;
	for (y = 0; y < rows_n; y++) {
		for (x = 0; x < columns_n; x++) {
			int val = rand_int(val_max)+1;
			sums_y[y] += val;
			sums_x[x] += val;
		}
		total += sums_y[y];
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
		int y1 = rand_int(rows_n), x1 = rand_int(columns_n), y2 = rand_int(rows_n), x2 = rand_int(columns_n);
		if (y1 != y2 && x1 != x2) {
			int cell_dec1 = y1*columns_n+x1, cell_dec2 = y2*columns_n+x2, offset_max;
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
				cells[y1*columns_n+x2].weight_int += offset;
				cells[y2*columns_n+x1].weight_int += offset;
			}
		}
	}
	print_matrix("AFTER RANDOM CHANGES");
	free(corrections);
	free(cells);
	free(sums_x);
	free(sums_y);
	return EXIT_SUCCESS;
}

void set_cell_and_weight_sums(cell_t *cell, int y, int x) {
	cell->weight = (double)sums_y[y]*sums_x[x]/total;
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
