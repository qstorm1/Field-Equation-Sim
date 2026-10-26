#include "matrix.h"
#include <math.h>


Matrix* initMatrix(int rows,int columns)				// Initialize a matrix
{
	Matrix *matrix = NULL;
	if (rows>0 && columns>0)
	{
		matrix = (Matrix*)malloc(sizeof(Matrix));
		if (matrix == NULL) {
			fprintf(stderr, "Memory allocation failed\n");
			return NULL;
		}
		matrix->rows = rows;
		matrix->columns = columns;
		matrix->data = (double*)malloc(sizeof(double)*rows*columns);

		if (matrix->data == NULL) {
			fprintf(stderr, "Memory allocation failed\n");
			free(matrix); // Free the already-allocated matrix struct memory
			return NULL;
		}
		memset(matrix->data, 0, sizeof(double)*rows*columns);
		return matrix;
	}
	else
		return NULL;
}

// Ensure the size of array matches matrix.row*columns
void assignMatrix(Matrix *matrix,double *array) 		// Assign values to the matrix
{
	if (matrix->data != NULL && array != NULL)
	{
		memcpy(matrix->data, array, (matrix->rows * matrix->columns) * sizeof(double));
	}
}

int capacityMatrix(Matrix *matrix)
{
	return matrix->rows * matrix->columns;
}

void freeMatrix(Matrix *matrix)
{
	if (matrix != NULL) {
        //free(matrix->data); // Free the matrix's data storage
		SAFE_FREE(matrix->data); // Safely free the matrix's data storage
        free(matrix); // Free the matrix struct itself
        printf("ptr released successfully\n");
    }
}

void copyMatrix(Matrix *matrix_A, Matrix *matrix_B)
{
	if (matrix_B == NULL || matrix_A == NULL) return;
    matrix_B->rows = matrix_A->rows;
    matrix_B->columns = matrix_A->columns;
    if (matrix_B->data != NULL) {
        SAFE_FREE(matrix_B->data); // Free the old memory
    }
    matrix_B->data = (double*)malloc(capacityMatrix(matrix_A) * sizeof(double));
    if (matrix_B->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    memcpy(matrix_B->data, matrix_A->data, capacityMatrix(matrix_A) * sizeof(double));
}

// Slice-index vector A[left:right] [left-1, right)
/*
Matrix* indexVector(Matrix *vector_A, int left, int right){
	Matrix* vector_slice = NULL;

	if (vector_slice == NULL || vector_A == NULL) return;
		fprintf(stderr, "Memory allocation failed or the vector is Null\n");

	vector_slice = initMatrix();

}
*/

void printMatrix(Matrix *matrix)
{
	int i =0;
	int j =0;
	for (i=0; i<matrix->rows; i++)
	{
		for(j=0; j<matrix->columns; j++){
			printf("%lf\t", matrix->data[i * matrix->columns + j]);
		}
		//if (i != matrix->rows -1)
			printf("\n");
	}
}

// Create a diagonal matrix
Matrix* diagMatrix(double* diag_elements, int n) {
    // Create an n x n matrix with all elements initialized to zero
	int i = 0;
    Matrix* result = initMatrix(n, n); // Initialize the matrix using initMatrix

    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Set the elements of diag_elements onto the diagonal of the result matrix
    for (i = 0; i < n; i++) {
        result->data[i * n + i] = diag_elements[i];  // Set the diagonal element
    }

    return result;
}


// Addition
Matrix* addMatrix(Matrix *matrix_A,Matrix *matrix_B)
{
	int i =0;
	int j =0;
	Matrix *matrix_C = NULL;

	if (matrix_A->rows == matrix_B->rows && matrix_A->columns == matrix_B->columns)
	{
		matrix_C = initMatrix(matrix_A->rows,matrix_A->columns);
		if (matrix_C == NULL) {
            fprintf(stderr, "Memory allocation failed for matrix_C\n");
            return NULL;
        }
		for (i=0;i<matrix_A->rows;i++)
		{
			for (j=0;j<matrix_A->columns;j++)
			{
				matrix_C->data[i*matrix_C->columns + j] = \
				matrix_A->data[i*matrix_A->columns + j] + matrix_B->data[i*matrix_B->columns + j];
			}
		}
		return matrix_C;
	}
	else
	{
		printf("矩阵形状不一致..\n");
		return NULL;
	}
}

// Subtraction
Matrix* subMatrix(Matrix *matrix_A,Matrix *matrix_B)
{
	int i =0;
	int j =0;
	Matrix *matrix_C = NULL;

	if (matrix_A->rows == matrix_B->rows && matrix_A->columns == matrix_B->columns)
	{
		matrix_C = initMatrix(matrix_A->rows,matrix_A->columns);
		if (matrix_C == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return NULL;
        }
		for (i=0;i<matrix_A->rows;i++)
		{
			for (j=0;j<matrix_A->columns;j++)
			{
				matrix_C->data[i*matrix_C->columns + j] = \
				matrix_A->data[i*matrix_A->columns + j] - matrix_B->data[i*matrix_B->columns + j];
			}
		}
		return matrix_C;
	}
	else
	{
		printf("矩阵形状不一致..\n");
		return NULL;
	}
}

// Matrix multiplication
Matrix* mulMatrix(Matrix *matrix_A,Matrix *matrix_B)
{
	int i =0;
	int j =0;
	int k =0;
	double sum = 0.0;
	Matrix *matrix_C = NULL;

	if (matrix_A->columns == matrix_B->rows)		// columns == rows
	{
		matrix_C = initMatrix(matrix_A->rows, matrix_B->columns);
		if (matrix_C == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return NULL;
        }
		for (i=0; i<matrix_A->rows; i++)
		{
			for (j=0; j<matrix_B->columns; j++)
			{
				sum = 0.0;
				for (k=0; k<matrix_A->columns; k++)
				{
					sum += matrix_A->data[i * matrix_A->columns + k] * matrix_B->data[k * matrix_B->rows + j];
				}
				matrix_C->data[i*matrix_C->columns + j] = sum;
			}
		}
		return matrix_C;
	}
	else
	{
		printf("不可相乘\n");
		return NULL;
	}
}

// Element-wise product
Matrix* dotProductMatrix(Matrix *matrix_A, Matrix *matrix_B) {
	int i = 0;
	int j = 0;
	Matrix *matrix_C = NULL;

    // Check whether matrices A and B have the same shape
    if (matrix_A->rows != matrix_B->rows || matrix_A->columns != matrix_B->columns) {
        printf("矩阵A和矩阵B的形状不一致，无法进行点积运算\n");
        return NULL;
    }

    // Initialize result matrix C, with the same number of rows and columns as A and B
    matrix_C = initMatrix(matrix_A->rows, matrix_A->columns);
    if (matrix_C == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Perform element-wise multiplication (Hadamard product)
    for (i = 0; i < matrix_A->rows; i++) {
        for (j = 0; j < matrix_A->columns; j++) {
            matrix_C->data[i * matrix_C->columns + j] =
                matrix_A->data[i * matrix_A->columns + j] * matrix_B->data[i * matrix_B->columns + j];
        }
    }

    return matrix_C;
}


// Extract a submatrix
Matrix* getsubMatrix(Matrix *matrix, int row, int col) {
	int i = 0;
	int j = 0;
	int minorRow = 0, minorCol = 0;
	Matrix *minor = NULL;

    // Parameter check
    if (!matrix || matrix->rows <= 1 || matrix->columns <= 1) {
        fprintf(stderr, "Error: Invalid matrix or matrix dimensions are too small.\n");
        return NULL;
    }
    if (row < 0 || row >= matrix->rows || col < 0 || col >= matrix->columns) {
        fprintf(stderr, "Error: Row or column index out of bounds.\n");
        return NULL;
    }

    // Create the new submatrix
    minor = initMatrix(matrix->rows - 1, matrix->columns - 1);
    if (!minor) {
        fprintf(stderr, "Error: Memory allocation failed for minor matrix.\n");
        return NULL;
    }

    for (i = 0; i < matrix->rows; i++) {
        for (j = 0; j < matrix->columns; j++) {
            if (i != row && j != col) {
                // Copy the elements outside the specified row and column
                minor->data[minorRow * minor->columns + minorCol] = matrix->data[i * matrix->columns + j];
                minorCol++;

                // If a row is filled, reset the column index and move on to the next row
                if (minorCol == minor->columns) {
                    minorCol = 0;
                    minorRow++;
                }
            }
        }
    }

    return minor;
}

// Compute the matrix determinant
double detMatrix(Matrix *matrix) {
	double det = 0.0;
	int col = 0;
	double minorDet = 0.0;
	double cofactor = 0.0;

    // Parameter check
    if (!matrix) {
        fprintf(stderr, "Error: Null matrix passed to Determinant.\n");
        return 0.0;
    }
    if (matrix->rows != matrix->columns) {
        fprintf(stderr, "Error: Non-square matrix has no determinant.\n");
        return 0.0;
    }
    if (matrix->rows == 0) {
		fprintf(stderr, "Error: The matrix has no element.\n");
        return 0.0; // Empty matrix
    }

    // Base case: 1x1 matrix
    if (matrix->rows == 1) {
        return matrix->data[0];
    }

    // Base case: 2x2 matrix
    if (matrix->rows == 2) {
        return matrix->data[0] * matrix->data[3] - matrix->data[1] * matrix->data[2];
    }

    // General case: compute recursively

    for (col = 0; col < matrix->columns; col++) {
        // Get the submatrix
        Matrix *minor = getsubMatrix(matrix, 0, col);
        if (!minor) {
            fprintf(stderr, "Error: Failed to compute minor matrix.\n");
            return 0.0;
        }

        // Recursively compute the determinant of the submatrix
        minorDet = detMatrix(minor);
        freeMatrix(minor);

        // Use the Laplace expansion formula
        cofactor = matrix->data[col]; // Pivot element
        if (col % 2 != 0) {
            cofactor = -cofactor; // Alternate the sign
        }
        det += cofactor * minorDet;
    }

    return det;
}

// Matrix transpose
void transMatrix(Matrix *matrix)			// Square matrix
{
	int i =0;
	int j =0;

	if (matrix->rows == matrix->columns)
	{
		Matrix *matrixTemp = initMatrix(matrix->rows,matrix->columns);       	// Create a temporary matrix
		if (matrixTemp == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return;
        }
		copyMatrix(matrix,matrixTemp);

		for (i = 0; i < matrix->rows; i++)
        {
            for (j = 0; j < matrix->columns; j++)
            {
                matrix->data[i * matrix->columns + j] = matrixTemp->data[j * matrix->columns + i];
            }
        }
		// Free the memory of the temporary matrix
        freeMatrix(matrixTemp);
	}
	else
	{
		printf("转置的矩阵必须为方阵\n");
	}
}

// Matrix transpose, non-square matrix
Matrix* transMatrix2(Matrix *matrix) // Transpose of a non-square matrix
{
	int i =0;
	int j =0;
	Matrix *matrix_T = NULL;

	if (matrix->rows == matrix->columns){
		transMatrix(matrix);
		return matrix;
	}

    if (matrix->rows != matrix->columns)
    {
        // Create a new Matrix struct to store the transposed matrix
        matrix_T = initMatrix(matrix->columns, matrix->rows);
        if (matrix_T == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return NULL;
        }

        for (int i = 0; i < matrix->columns; i++)
        {
            for (int j = 0; j < matrix->rows; j++)
            {
                matrix_T->data[i * matrix_T->columns + j] = matrix->data[j * matrix->columns + i];
            }
        }
        return matrix_T;
    }
    else
    {
        printf("输入的矩阵是方阵，不需要转置\n");
        return NULL;
    }
}

// Whether a 2nd/3rd-order matrix is invertible
bool isInvertible(Matrix *matrix) {
	double det = 0.0;

    if (matrix->rows != matrix->columns) {
        return 0; // A non-square matrix is not invertible
    }

    // Determinant of a 2x2 matrix
    if (matrix->rows == 2) {
        det = matrix->data[0] * matrix->data[3] - matrix->data[1] * matrix->data[2];
        return fabs(det) > 1e-9;  // Check whether the determinant is close to 0
    }

    // Determinant of a 3x3 matrix
    if (matrix->rows == 3) {
        det = matrix->data[0] * (matrix->data[4] * matrix->data[8] - matrix->data[5] * matrix->data[7]) -
                     matrix->data[1] * (matrix->data[3] * matrix->data[8] - matrix->data[5] * matrix->data[6]) +
                     matrix->data[2] * (matrix->data[3] * matrix->data[7] - matrix->data[4] * matrix->data[6]);
        return fabs(det) > 1e-9;
    }

    // Larger matrices require a more complex algorithm
    return false; // Currently only 2x2 and 3x3 matrices are supported
}

// Function to determine whether an nth-order matrix is invertible
bool isInvertiblenx_n(Matrix *matrix) {
	double det = 0.0;

    if (matrix == NULL || matrix->rows != matrix->columns) {
        // A non-square or empty matrix is directly treated as not invertible
		fprintf(stderr, "Memory allocation failed or matrix is non-square\n");
        return false;
    }

    det = detMatrix(matrix);
    // If the determinant is 0, the matrix is not invertible; otherwise, it is invertible
    return !FLOAT_EQUAL(det, 0.0);
}

// Matrix inverse, 2nd/3rd order
Matrix* invMatrix(Matrix *matrix) {
	Matrix *inverse = NULL;
	double det = 0.1; // Ensure division is safe
	double inv_det = 0.0;

    if (!isInvertible(matrix)) {
        return NULL;
    }

    inverse = initMatrix(matrix->rows, matrix->columns);
    if (inverse == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    // Inverse of a 2x2 matrix
    if (matrix->rows == 2) {
        det = matrix->data[0] * matrix->data[3] - matrix->data[1] * matrix->data[2];
        if (det == 0) {
            return NULL;  // Prevent division by zero
        }
        inverse->data[0] = matrix->data[3] / det;
        inverse->data[1] = -matrix->data[1] / det;
        inverse->data[2] = -matrix->data[2] / det;
        inverse->data[3] = matrix->data[0] / det;
    }

    // Inverse of a 3x3 matrix
    if (matrix->rows == 3) {
        det = matrix->data[0] * (matrix->data[4] * matrix->data[8] - matrix->data[5] * matrix->data[7]) -
                     matrix->data[1] * (matrix->data[3] * matrix->data[8] - matrix->data[5] * matrix->data[6]) +
                     matrix->data[2] * (matrix->data[3] * matrix->data[7] - matrix->data[4] * matrix->data[6]);
        if (det == 0) {
            return NULL;  // Prevent division by zero
        }
        inv_det = 1 / det;

        inverse->data[0] = (matrix->data[4] * matrix->data[8] - matrix->data[5] * matrix->data[7]) * inv_det;
        inverse->data[1] = -(matrix->data[1] * matrix->data[8] - matrix->data[2] * matrix->data[7]) * inv_det;
        inverse->data[2] = (matrix->data[1] * matrix->data[5] - matrix->data[2] * matrix->data[4]) * inv_det;
        inverse->data[3] = -(matrix->data[3] * matrix->data[8] - matrix->data[5] * matrix->data[6]) * inv_det;
        inverse->data[4] = (matrix->data[0] * matrix->data[8] - matrix->data[2] * matrix->data[6]) * inv_det;
        inverse->data[5] = -(matrix->data[0] * matrix->data[5] - matrix->data[3] * matrix->data[2]) * inv_det;
        inverse->data[6] = (matrix->data[3] * matrix->data[7] - matrix->data[4] * matrix->data[6]) * inv_det;
        inverse->data[7] = -(matrix->data[0] * matrix->data[7] - matrix->data[1] * matrix->data[6]) * inv_det;
        inverse->data[8] = (matrix->data[0] * matrix->data[4] - matrix->data[1] * matrix->data[3]) * inv_det;
    }

    return inverse;
}

// Safe Gaussian elimination for the inverse of an nth-order matrix
Matrix* safeMatrixInv(Matrix *matrix, int n) {
	int i, j, k, principal;
	double* aug = NULL; // Augmented matrix
	double max = 0.0;
	double tmp = 0.0;
	double factor = 0.0;
	double diag_factor = 0.0;
	Matrix* inverse = NULL;

    if (matrix == NULL || n <= 0) {
        fprintf(stderr, "Invalid input matrix.\n");
        return NULL;
    }

	aug = (double*)malloc(sizeof(double) * n * n * 2); // Augmented matrix
    if (aug == NULL) {
        fprintf(stderr, "Memory allocation failed for augmented matrix.\n");
        return NULL;
    }

    // Initialize the augmented matrix
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            aug[i * 2 * n + j] = matrix->data[i * n + j]; // Original matrix part
            aug[i * 2 * n + (j + n)] = (i == j) ? 1.0 : 0.0; // Identity matrix part
        }
    }

    // Gaussian elimination
    for (j = 0; j < n; j++) {
        // Select the pivot
        principal = j;
        max = fabs(aug[principal * 2 * n + j]);
        for (i = j + 1; i < n; i++) {
            if (fabs(aug[i * 2 * n + j]) > max) {
                principal = i;
                max = fabs(aug[i * 2 * n + j]);
            }
        }

        // Check whether the pivot is 0
        if (fabs(max) < 1e-9) {
            fprintf(stderr, "Matrix is singular or nearly singular.\n");
            SAFE_FREE(aug);
            return NULL;
        }

        // Swap rows
        if (j != principal) {
            for (k = 0; k < 2 * n; k++) {
                tmp = aug[j * 2 * n + k];
                aug[j * 2 * n + k] = aug[principal * 2 * n + k];
                aug[principal * 2 * n + k] = tmp;
            }
        }

        // Reduce to row echelon form
        for (i = 0; i < n; i++) {
            if (i != j) {
                factor = aug[i * 2 * n + j] / aug[j * 2 * n + j];
                for (k = 0; k < 2 * n; k++) {
                    aug[i * 2 * n + k] -= factor * aug[j * 2 * n + k];
                }
            }
        }

        // Normalize the main diagonal to 1
        diag_factor = aug[j * 2 * n + j];
        for (k = 0; k < 2 * n; k++) {
            aug[j * 2 * n + k] /= diag_factor;
        }
    }

    // Extract the inverse matrix
	inverse = initMatrix(n, n);
    if (inverse == NULL) {
        fprintf(stderr, "Memory allocation failed for inverse matrix.\n");
        SAFE_FREE(aug);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            inverse->data[i * n + j] = aug[i * 2 * n + (j + n)];
        }
    }

    SAFE_FREE(aug); // Free the augmented matrix
    return inverse;
}


// Matrix division, right division, 2nd/3rd order
Matrix* rightDivMatrix(Matrix *matrix_A, Matrix *matrix_B) {
	Matrix *inverse_B = NULL;
	Matrix *result = NULL;

    // Check whether B is a square matrix
    if (matrix_B->rows != matrix_B->columns) {
        printf("矩阵 B 必须是方阵\n");
        return NULL;
    }

    // Check whether A is invertible
    if (!isInvertible(matrix_A)) {
        printf("矩阵 A 不可逆\n");
        return NULL;
    }

    // Compute the inverse of B
    inverse_B = invMatrix(matrix_B);
    if (inverse_B == NULL) {
        printf("计算矩阵 B 的逆矩阵失败\n");
        return NULL;
    }

    // Compute A * B^-1 to get the result X
    result = mulMatrix(matrix_A, inverse_B);
    if (result == NULL) {
        printf("矩阵乘法失败\n");
        freeMatrix(inverse_B);
        return NULL;
    }

    // Free the memory of the intermediate result
    freeMatrix(inverse_B);

    return result;
}


// Matrix division, left division, 2nd/3rd order
Matrix* leftDivMatrix(Matrix *matrix_A, Matrix *matrix_B) {
	Matrix *inverse_A = NULL;
	Matrix *result = NULL;

    // Check whether A is a square matrix
    if (matrix_A->rows != matrix_A->columns) {
        printf("矩阵 A 必须是方阵\n");
        return NULL;
    }

    // Check whether A is invertible
    if (!isInvertible(matrix_A)) {
        printf("矩阵 A 不可逆\n");
        return NULL;
    }

    // Compute the inverse of A
    inverse_A = invMatrix(matrix_A);
    if (inverse_A == NULL) {
        printf("计算矩阵 A 的逆矩阵失败\n");
        return NULL;
    }

    // Compute A^-1 * B to get the result X
    result = mulMatrix(inverse_A, matrix_B);
    if (result == NULL) {
        printf("矩阵乘法失败\n");
        freeMatrix(inverse_A);
        return NULL;
    }

    // Free the memory of the intermediate result
    freeMatrix(inverse_A);

    return result;
}

// Left division for an nth-order matrix, no square-matrix restriction
Matrix* leftDivMatrix_n(Matrix *matrix_A,Matrix *matrix_B){
	Matrix *inverse_A = NULL;
	Matrix *result = NULL;

	// Check whether A is a square matrix
    if (matrix_A->rows != matrix_A->columns) {
        printf("矩阵 A 必须是方阵\n");
        return NULL;
    }

    // Check whether A is invertible
    if (!isInvertiblenx_n(matrix_A)) {
        printf("矩阵 A 不可逆\n");
        return NULL;
    }

    // Compute the inverse of A
	inverse_A = safeMatrixInv(matrix_A, matrix_A->rows);
    if (inverse_A == NULL) {
        printf("计算矩阵 A 的逆矩阵失败\n");
        return NULL;
    }

    // Compute A^-1 * B to get the result X
    result = mulMatrix(inverse_A, matrix_B);
    if (result == NULL) {
        printf("矩阵乘法失败\n");
        freeMatrix(inverse_A);
        return NULL;
    }

    // Free the memory of the intermediate result
    freeMatrix(inverse_A);

    return result;
}

// Right division for an nth-order matrix, no square-matrix restriction
Matrix* rightDivMatrix_n(Matrix *matrix_A,Matrix *matrix_B){
	Matrix *inverse_B = NULL;
	Matrix *result = NULL;

	// Check whether B is a square matrix
    if (matrix_B->rows != matrix_B->columns) {
        printf("矩阵 B 必须是方阵\n");
        return NULL;
    }

    // Check whether B is invertible
    if (!isInvertiblenx_n(matrix_B)) {
        printf("矩阵 B 不可逆\n");
        return NULL;
    }

    // Compute the inverse of B
	inverse_B = safeMatrixInv(matrix_B, matrix_B->rows);
    if (inverse_B == NULL) {
        printf("计算矩阵 B 的逆矩阵失败\n");
        return NULL;
    }

    // Compute A * B^-1 to get the result X
    result = mulMatrix(matrix_A, inverse_B);
    if (result == NULL) {
        printf("矩阵乘法失败\n");
        freeMatrix(inverse_B);
        return NULL;
    }

    // Free the memory of the intermediate result
    freeMatrix(inverse_B);

    return result;
}

// Vector cross product
Matrix* crossProduct(Matrix *vector_A, Matrix *vector_B) {
	Matrix *result = NULL;

    // Assume vector_A and vector_B are 3x1 matrices
    if (vector_A->rows != 3 || vector_A->columns != 1 || vector_B->rows != 3 || vector_B->columns != 1) {
        printf("Only 3D vectors are supported for cross product.\n");
        return NULL;
    }

    result = initMatrix(3, 1);
    if (result == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    // Compute each component of the cross product
    result->data[0] = vector_A->data[1] * vector_B->data[2] - vector_A->data[2] * vector_B->data[1]; // x
    result->data[1] = vector_A->data[2] * vector_B->data[0] - vector_A->data[0] * vector_B->data[2]; // y
    result->data[2] = vector_A->data[0] * vector_B->data[1] - vector_A->data[1] * vector_B->data[0]; // z

    return result;
}

// Matrix cross product. A and B must each have at least one dimension of length 3.
Matrix* crossMatrix(Matrix *matrix_A, Matrix *matrix_B) {
	Matrix *result = NULL;
	int i = 0;
	Matrix vec_A;
	Matrix vec_B;
	Matrix *cross = NULL;
    // Check whether the matrices meet the cross product requirements: 3xN or Nx3
    if (matrix_A->columns == 3 && matrix_B->columns == 3) {
        // Cross product of 3x3 matrices
        if (matrix_A->rows != matrix_B->rows) {
            printf("矩阵的行数不匹配，无法进行叉乘\n");
            return NULL;
        }
        result = initMatrix(matrix_A->rows, 3);
        if (result == NULL) {
			fprintf(stderr, "Memory allocation failed!\n");
            return NULL;
        }

        // Compute the vector cross product for each row
        for (i = 0; i < matrix_A->rows; i++) {
           //Matrix vec_A = {3, 1, &matrix_A->data[i * 3]};
           //Matrix vec_B = {3, 1, &matrix_B->data[i * 3]};
			vec_A.rows = 3;
            vec_A.columns = 1;
            vec_A.data = &matrix_A->data[i * 3];
            vec_B.rows = 3;
            vec_B.columns = 1;
            vec_B.data = &matrix_B->data[i * 3];
            cross = crossProduct(&vec_A, &vec_B);
            if (cross == NULL) {
				fprintf(stderr, "Memory allocation failed!\n");
                freeMatrix(result);
                return NULL;
            }
            memcpy(&result->data[i * 3], cross->data, 3 * sizeof(double));
            freeMatrix(cross);
        }
        return result;
    } else if (matrix_A->columns == 1 && matrix_B->columns == 1) {
        // Vector cross product (3x1 matrices)
        if (matrix_A->rows != 3 || matrix_B->rows != 3) {
            printf("只能对3D列向量进行叉乘。\n");
            return NULL;
        }
        return crossProduct(matrix_A, matrix_B);
    } else {
        printf("不支持的矩阵形状。\n");
        return NULL;
    }
}

// Compute the 1-norm of a vector
Matrix* normVector1(Matrix *vector) {
	Matrix *norm = NULL;
	int i = 0;
	double sum = 0.0;

    if (vector->columns != 1) {
        return NULL; // Only row vectors (single column) are supported
    }

    norm = initMatrix(1, 1); // The 1-norm result is a scalar
    if (!norm) {
        return NULL;
    }

    for (i = 0; i < vector->rows; i++) {
        sum += fabs(vector->data[i]);
    }

    norm->data[0] = sum;
    return norm;
}

// Compute the 2-norm of a vector
Matrix* normVector2(Matrix *vector) {
	Matrix *norm = NULL;
	int i = 0;
	double sum = 0.0;

    if (vector->columns != 1) {
        return NULL; // Only row vectors (single column) are supported
    }

    norm = initMatrix(1, 1); // The 2-norm result is a scalar
    if (!norm) {
        return NULL;
    }

    for (i = 0; i < vector->rows; i++) {
        sum += vector->data[i] * vector->data[i];
    }

    norm->data[0] = sqrt(sum);
    return norm;
}


// Rotation matrix about the x-axis
Matrix* rotationMatrix_x (double element){
    Matrix* result = initMatrix(3, 3);

    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    // Set the elements of elements onto the diagonal of the result matrix
	result->data[0] = 1;
	result->data[1] = 0;
	result->data[2] = 0;

	result->data[3] = 0;
	result->data[4] = cos(element);
	result->data[5] = sin(element);

	result->data[6] = 0;
	result->data[7] = -sin(element);
	result->data[8] = cos(element);
    return result;
}

// Rotation matrix about the y-axis
Matrix* rotationMatrix_y (double element){
	Matrix* result = initMatrix(3, 3);

    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    // Set the elements of elements onto the diagonal of the result matrix
	result->data[0] = cos(element);
	result->data[1] = 0;
	result->data[2] = -sin(element);

	result->data[3] = 0;
	result->data[4] = 1;
	result->data[5] = 0;

	result->data[6] = sin(element);
	result->data[7] = 0;
	result->data[8] = cos(element);
    return result;
}

// Rotation matrix about the z-axis
Matrix* rotationMatrix_z (double element){
	Matrix* result = initMatrix(3, 3);

    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    // Set the elements of elements onto the diagonal of the result matrix
	result->data[0] = cos(element);
	result->data[1] = sin(element);
	result->data[2] = 0;

	result->data[3] = -sin(element);
	result->data[4] = cos(element);
	result->data[5] = 0;

	result->data[6] = 0;
	result->data[7] = 0;
	result->data[8] = 1;
    return result;
}