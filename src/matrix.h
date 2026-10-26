/**********************************************************
** FreeMatrix-C: An open source C/C++ matrix and vector library for Safe and High Performance Computations(support version C99+)
** FreeMatrix-C 是一个由Gao，Xing开发的C语言矩阵运算库，它提供了安全、高效的矩阵操作函数与矩阵算法，包括矩阵生成、基础运算、逆运算、行列式、范数运算、秩、奇异值分解等。
** 该项目的目标是简化C语言环境下的矩阵运算，语法支持早期C语言版本及早期Microsoft Visual Studio 2010编译器，便于科研和教学使用。
** 该项目遵循Apache-3.0许可证，可在GitHub地址上找到完整源代码与测试用例。

** Last revised date: 2024-12-9
** Author: Gao，Xing
** Email: xinggao163@163.com

** 测试平台：
** 操作系统：Windwos 11 
** 编译器：Microsoft Visual Studio 2010， _MSC_VER >= 1600
**********************************************************/
/**********************************************************
** Refs 
** [1] https://github.com/PX4/PX4-Matrix
** [2] https://github.com/EricLengyel/Terathon-Math-Library
** [3] https://gitcode.com/gh_mirrors/ma/Matrix_hub
** [4] https://learn.microsoft.com/cpp/overview/compiler-versions
**********************************************************/
#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 一Dimensional pointer version

// Macros for safely releasing memory
#define SAFE_FREE(ptr) do { if (ptr) { free(ptr); ptr = NULL; } } while (0)
//Custom precision two-number comparison to replace the == operator
#define FLOAT_EQUAL(a, b) (fabs((a) - (b)) < 1e-6)

// Data structure for storing matrices
typedef struct 
{
	int rows,columns;
	double *data; // Improve indexing efficiency
}Matrix;

//Basic operations on matrices
Matrix* initMatrix(int rows, int columns); // Initializes a matrix.
void assignMatrix(Matrix *matrix, double *array); // Assigns values to a matrix.
int capacityMatrix(Matrix *matrix); // Gets the capacity of a matrix.
void freeMatrix(Matrix *matrix); // Frees a matrix.
void copyMatrix(Matrix *matrix_A, Matrix *matrix_B); // Copies the values of matrix A to B.
Matrix* indexVector(Matrix *vector_A, int left, int right); //Index vector A[left:right] [left-1, right)
Matrix* indexMatrix(Matrix *matrix_A, int left, int right); //Index matrix A[left:right] [left-1, right)
void printMatrix(Matrix *matrix); // Prints a matrix

// Basic Matrix Operations
Matrix* diagMatrix(double* diag_elements, int n);  // Create a diagonal matrix
Matrix* addMatrix(Matrix *matrix_A, Matrix *matrix_B);		// Matrix addition
Matrix* subMatrix(Matrix *matrix_A, Matrix *matrix_B);		// Matrix subtraction
Matrix* mulMatrix(Matrix *matrix_A, Matrix *matrix_B);		// Matrix multiplication
Matrix* rightDivMatrix(Matrix *matrix_A, Matrix *matrix_B);		// Matrix division (2nd–3rd order): right division (/), where A/B = A * B⁻¹
Matrix* leftDivMatrix(Matrix *matrix_A, Matrix *matrix_B);		// Matrix division (2nd–3rd order): left division (\), where A\B = A⁻¹ * B
Matrix* rightDivMatrix_n(Matrix *matrix_A, Matrix *matrix_B);		// Matrix division (n-th order): right division (/), where A/B = A * B⁻¹
Matrix* leftDivMatrix_n(Matrix *matrix_A, Matrix *matrix_B);		// Matrix division (n-th order): left division (\), where A\B = A⁻¹ * B

Matrix* dotProductMatrix(Matrix *matrix_A, Matrix *matrix_B); // Matrix dot product
Matrix* crossProduct(Matrix *vector_A, Matrix *vector_B); // Vector cross product
Matrix* crossMatrix(Matrix *matrix_A, Matrix *matrix_B); // Matrix cross product

Matrix* getsubMatrix(Matrix *matrix, int row, int col); // Get a submatrix (by removing the specified row and column)
double detMatrix(Matrix *matrix); // Calculate the determinant of an n-th order matrix

void transMatrix(Matrix *matrix);			// Transpose (for square matrices)
Matrix* transMatrix2(Matrix *matrix);  // Transpose (for non-square matrices)

bool isInvertible(Matrix *matrix); // Check if a matrix is invertible

#endif // MATRIX_H
