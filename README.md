# MatrixFromSums
Generate random matrix from row/column sums filled with values >= 1
 
Parameters expected on standard input:
- number of rows > 0
- number of columns > 0
- maximum value > 0

With number of rows >= number of columns

The row/column sums (sums_y, sums_x below) are generated automatically from random values between 1 and \<maximum value\> but the program can be easily modified to read them from the standard input.

First the value for row i and column j is set to floor(sums_y\[i\]\*sums_x\[j\]/total) (sum of all values). The sum of these original values for each row/column has a "debt" >= 0 compared to the target sum.

The debt is reimbursed row by row by incrementing the original values in the columns that have the greatest debt.

Finally <number of rows>\*<number of columns> offsets are applied to the matrix values by choosing two random cells at position [y1, x1] and [y2, x2]. The offset is a random value between 1 and the minimum value between these random cells. Cells at position [y1, x1] and [y2, x2] have their value decremented by this offset and cells at position [y1, x2] and [y2, x1] have their value incremented by this offset to keep the sum constraints respected.
