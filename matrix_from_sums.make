MATRIX_FROM_SUMS_C_FLAGS=-O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings
MATRIX_FROM_SUMS_OBJS=matrix_from_sums.o mtrand.o

matrix_from_sums: ${MATRIX_FROM_SUMS_OBJS}
	gcc -o matrix_from_sums ${MATRIX_FROM_SUMS_OBJS} -lm

matrix_from_sums.o: matrix_from_sums.c matrix_from_sums.make
	gcc -c ${MATRIX_FROM_SUMS_C_FLAGS} -o matrix_from_sums.o matrix_from_sums.c

mtrand.o: mtrand.h mtrand.c matrix_from_sums.make
	gcc -c ${MATRIX_FROM_SUMS_C_FLAGS} -o mtrand.o mtrand.c

clean:
	rm -f matrix_from_sums ${MATRIX_FROM_SUMS_OBJS}
