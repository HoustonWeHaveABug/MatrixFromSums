# MatrixFromSums
Generate random matrix filled with values >= 1 from row/column sums

Parameters expected on standard input:
- rows_n (number of rows) > 0
- columns_n (number of columns) > 0
- val_max (maximum value) >= 0

With rows_n >= columns_n and rows_n\*columns_n <= INT_MAX

If val_max is > 0 then row/column sums are generated from random values between 1 and val_max, otherwise row/column sums are read from standard input.

First the value for row i and column j is set to floor(sums_y\[i\]\*sums_x\[j\]/total) (sum of all values). The sum of these original values for each row/column has a "debt" >= 0 compared to the target sum.

Then the debt is reimbursed row by row by incrementing the original values in the columns that have the greatest debt.

Finally \<number of rows\>\*\<number of columns\> offsets are applied to the matrix values by choosing two random cells at position \[y1, x1\] and \[y2, x2\]. The offset is a random value between 1 and the minimum value between these random cells. Cells at position \[y1, x1\] and \[y2, x2\] have their value decremented by this offset and cells at position \[y1, x2\] and \[y2, x1\] have their value incremented by this offset to keep the sum constraints respected.
