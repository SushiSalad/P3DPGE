#pragma once
#include "Debug.h"

struct Vector3;

/*
	!!!! Row-Major Matrices !!!!

	Non-Matrix vs Matrix interactions should be in Math.h
	Acessing matrix values starts at zero for both the row and column: 0...n-1 not 1...n
*/

//TODO(o,delle) test that there is no memory leak on destruction of Matrix objects
struct Matrix {
	uint32 rows;
	uint32 cols;
	uint32 elementCount;
	float* data;

	Matrix(uint32 inRows, uint32 inCols);
	Matrix(uint32 inRows, uint32 inCols, std::vector<float> list);
	Matrix(const Matrix& m);
	Matrix(Vector3 v); //TODO(delle) define this in Math.h
	Matrix(Vector3 v, float w); //TODO(delle) define this in Math.h
	~Matrix();

	float&	operator () (uint32 row, uint32 col);
	void	operator =	(const Matrix& rhs);
	Matrix  operator *  (const float& rhs) const;
	void	operator *= (const float& rhs);
	Matrix  operator /  (const float& rhs) const;
	void	operator /= (const float& rhs);
	Matrix  operator +  (const Matrix& rhs) const;
	void	operator += (const Matrix& rhs);
	Matrix  operator -  (const Matrix& rhs) const;
	void	operator -= (const Matrix& rhs);
	Matrix  operator *  (const Matrix& rhs) const;
	void	operator *= (const Matrix& rhs);
	Matrix  operator ^  (const Matrix& rhs) const;
	void	operator ^= (const Matrix& rhs);
	Matrix  operator %  (const Matrix& rhs) const; 
	void	operator %= (const Matrix& rhs);
	bool	operator == (const Matrix& rhs) const;
	bool	operator != (const Matrix& rhs) const;
	friend Matrix operator* (const float& lhs, const Matrix& rhs) { return rhs * lhs; }

	const std::string str() const;
	const std::string str2F() const;
	void Copy(const Matrix& m);
	Matrix Transpose() const;
	Matrix Submatrix(std::vector<uint32> inRows, std::vector<uint32> inCols) const;
	float Minor(int row, int col) const;
	float Cofactor(int row, int col) const;
	Matrix Adjoint() const;
	float Determinant() const;
	Matrix Inverse() const;

	static Matrix Identity(uint32 rows, uint32 cols);
};



//// Constructors ////

inline Matrix::Matrix(uint32 inRows, uint32 inCols) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	this->data = new float[elementCount]();
}

inline Matrix::Matrix(uint32 inRows, uint32 inCols, std::vector<float> list) : rows(inRows), cols(inCols) {
	ASSERT(inRows != 0 && inCols != 0, "Matrix constructor was given zero size");
	this->elementCount = inRows * inCols;
	uint32 inCount = list.size();
	ASSERT(inCount <= elementCount, "Matrix constructor was given too many elements for given dimensions");
	this->data = new float[elementCount]();
	for (int i = 0; i < list.size(); ++i) {
		this->data[i] = list[i];
	}
}

inline Matrix::Matrix(const Matrix& m) {
	this->rows = m.rows;
	this->cols = m.cols;
	this->elementCount = rows * cols;
	this->data = new float[elementCount]();
	Copy(m);
}

inline Matrix::~Matrix() {
	delete[] data;
}



//// Operators ////

//element accessor: matrix(row,col)
inline float& Matrix::operator () (uint32 row, uint32 col) {
	ASSERT(row <= rows && col <= cols, "Matrix subscript out of bounds");
	return data[cols*row + col];
}

//deletes current data, copies properties from rhs, creates a new copy of the data from rhs
inline void	   Matrix::operator =  (const Matrix& rhs) {
	if (this->data != rhs.data) {
		this->rows = rhs.rows;
		this->cols = rhs.cols;
		this->elementCount = rows * cols;
		this->~Matrix();
		this->data = new float[elementCount]();
		Copy(rhs);
	}
}

//scalar multiplication
inline Matrix  Matrix::operator *  (const float& rhs) const {
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] *= rhs;
	}
	return newMatrix;
}

//scalar multiplication and assignment
inline void    Matrix::operator *= (const float& rhs) {
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] *= rhs;
	}
}

//scalar division
inline Matrix  Matrix::operator /  (const float& rhs) const {
	ASSERT(rhs != 0, "Matrix elements cant be divided by zero");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] /= rhs;
	}
	return newMatrix;
}

//scalar division and assignment
inline void    Matrix::operator /= (const float& rhs){
	ASSERT(rhs != 0, "Matrix elements cant be divided by zero");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] /= rhs;
	}
}

//element-wise addition
inline Matrix  Matrix::operator +  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix addition requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] += rhs.data[i];
	}
	return newMatrix;
}

//element-wise addition and assignment
inline void    Matrix::operator += (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix addition requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] += rhs.data[i];
	}
}

//element-wise substraction
inline Matrix  Matrix::operator -  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix subtraction requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] -= rhs.data[i];
	}
	return newMatrix;
}

//element-wise substraction and assignment
inline void    Matrix::operator -= (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix subtraction requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] -= rhs.data[i];
	}
}

//TODO(o,delle) look into optimizing this by transposing to remove a loop, see Unreal implementation
inline Matrix  Matrix::operator *  (const Matrix& rhs) const{
	ASSERT(cols == rhs.rows, "Matrix multiplication requires the columns of the left matrix to equal the rows of the right matrix");
	Matrix newMatrix(rows, rhs.cols);
	for (int i = 0; i < this->rows; ++i) { //i=m
		for (int j = 0; j < rhs.cols; ++j) { //j=p
			for (int k = 0; k < rhs.rows; ++k) { //k=n
				newMatrix.data[rhs.cols * i + j] += this->data[this->cols * i + k] * rhs.data[rhs.cols * k + j];
			}
		}
	}
	return newMatrix;
}

inline void    Matrix::operator *= (const Matrix& rhs){
	ASSERT(cols == rhs.rows, "Matrix multiplication requires the columns of the left matrix to equal the rows of the right matrix");
	Matrix newMatrix(rows, rhs.cols);
	for (int i = 0; i < this->rows; ++i) { //i=m
		for (int j = 0; j < rhs.cols; ++j) { //j=p
			for (int k = 0; k < rhs.rows; ++k) { //k=n
				newMatrix.data[rhs.cols * i + j] += this->data[this->cols * i + k] * rhs.data[rhs.cols * k + j];
			}
		}
	}
	*this = newMatrix;
}

//element-wise multiplication
inline Matrix  Matrix::operator ^  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise multiplication requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		newMatrix.data[i] *= rhs.data[i];
	}
	return newMatrix;
} 

//element-wise multiplication and assignment
inline void    Matrix::operator ^= (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise multiplication requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		this->data[i] *= rhs.data[i];
	}
}

//element-wise division
inline Matrix  Matrix::operator %  (const Matrix& rhs) const{
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise division requires the same dimensions");
	Matrix newMatrix(*this);
	for (int i = 0; i < newMatrix.elementCount; ++i) {
		ASSERT(rhs.data[i] != 0, "Matrix element-wise division doesnt allow zeros in the right matrix");
		newMatrix.data[i] /= rhs.data[i];
	}
	return newMatrix;
} 

//element-wise division and assignment
inline void    Matrix::operator %= (const Matrix& rhs){
	ASSERT(rows == rhs.rows && cols == rhs.cols, "Matrix element-wise division requires the same dimensions");
	for (int i = 0; i < elementCount; ++i) {
		ASSERT(rhs.data[i] != 0, "Matrix element-wise division doesnt allow zeros in the right matrix");
		this->data[i] /= rhs.data[i];
	}
}

inline bool	   Matrix::operator	== (const Matrix& rhs) const { 
	if (this->rows != rhs.rows || this->cols != rhs.cols || this->elementCount != rhs.elementCount) {
		return false;
	}
	for (int i = 0; i < elementCount; ++i) {
		if (this->data[i] != rhs.data[i]) {
			return false;
		}
	}
	return true;
}

inline bool    Matrix::operator	!= (const Matrix& rhs) const { 
	return !(*this == rhs); 
}



//// Functions ////

//TODO(c,delle) clean up Matrix.str() and Matrix.str2F()
const std::string Matrix::str() const {
	if (rows == 0 || cols == 0) {
		return "|Zero dimension matrix|";
	}

	std::string str = std::to_string(rows) + "x" + std::to_string(cols) + " Matrix:\n|";
	if (rows == 1) {
		for (int i = 0; i < cols-1; ++i) {
			char buffer[15];
			std::snprintf(buffer, 15, "%+.6f", data[i]);
			str += std::string(buffer) + ", ";
		}
		char buffer[15];
		std::snprintf(buffer, 15, "%+.6f", data[elementCount - 1]);
		str += std::string(buffer) + "|";
		return str;
	}

	for (int i = 0; i < elementCount-1; ++i) {
		char buffer[15];
		std::snprintf(buffer, 15, "%+.6f", data[i]);
		str += std::string(buffer);
		if ((i+1) % cols != 0) {
			str += ", ";
		} else {
			str += "|\n|";
		}
	}
	char buffer[15];
	std::snprintf(buffer, 15, "%+.6f", data[elementCount - 1]);
	str += std::string(buffer) + "|";
	return str;
};

const std::string Matrix::str2F() const {
	if (rows == 0 || cols == 0) {
		return "|Zero dimension matrix|";
	}

	std::string str = std::to_string(rows) + "x" + std::to_string(cols) + " Matrix:\n|";
	if (rows == 1) {
		for (int i = 0; i < cols - 1; ++i) {
			char buffer[15];
			std::snprintf(buffer, 15, "%+.2f", data[i]);
			str += std::string(buffer) + ", ";
		}
		char buffer[15];
		std::snprintf(buffer, 15, "%+.2f", data[elementCount - 1]);
		str += std::string(buffer) + "|";
		return str;
	}

	for (int i = 0; i < elementCount - 1; ++i) {
		char buffer[15];
		std::snprintf(buffer, 15, "%+.2f", data[i]);
		str += std::string(buffer);
		if ((i + 1) % cols != 0) {
			str += ", ";
		} else {
			str += "|\n|";
		}
	}
	char buffer[15];
	std::snprintf(buffer, 15, "%+.2f", data[elementCount - 1]);
	str += std::string(buffer) + "|";
	return str;
};

//copys the data from one matrix to the other
//REQUIRES both to have the same dimensions
inline void Matrix::Copy(const Matrix& m) {
	ASSERT(rows == m.rows && cols == m.cols, "Cant copy matrices of unequal dimensions");
	float* p = data + (elementCount);
	float* q = m.data + (elementCount);
	while (p > data) {
		*--p = *--q;
	}
}

//converts the rows into columns and vice-versa
inline Matrix Matrix::Transpose() const{
	Matrix newMatrix(cols, rows);
	for (int i = 0; i < elementCount; ++i) {
		newMatrix.data[i] = data[cols * (i%rows) + (i/rows)];
	}
	return newMatrix;
}

//returns a matrix only with the specified rows and cols
//NOTE 0...n-1 not 1...n
inline Matrix Matrix::Submatrix(std::vector<uint32> inRows, std::vector<uint32> inCols) const{
	ASSERT(inRows.size() != 0 && inCols.size() > 0, "Matrix submatrix cant be performed with zero dimensions");
	Matrix newMatrix(inRows.size(), inCols.size());
	for (int i = 0; i < inRows.size(); ++i) {
		for (int j = 0; j < inCols.size(); ++j) {
			newMatrix.data[newMatrix.cols * i + j] = data[cols * inRows[i] + inCols[j]];
		}
	}
	return newMatrix;
}

//returns the determinant of this matrix without the specified row and column
inline float Matrix::Minor(int row, int col) const {
	ASSERT(rows == cols, "Matrix minor can only be take of a square matrix");
	ASSERT(elementCount > 1, "Matrix minor cant be take of one-dimensional matrix");
	Matrix newMatrix(rows - 1, cols - 1);
	int index = 0;
	for (int i = 0; i < rows; ++i) {
		if (i == row) continue;
			for (int j = 0; j < cols; ++j) {
				if (j == col) continue;
					newMatrix.data[index++] = data[cols * i + j];
			}
	}
	return newMatrix.Determinant();
}

//returns the cofactor (minor with adjusted sign based on location in matrix) at given row and column
inline float Matrix::Cofactor(int row, int col) const{
	if ((row + col) % 2) {
		return -Minor(row, col);
	} else {
		return Minor(row, col);
	}
}

//returns the determinant of the matrix
inline float Matrix::Determinant() const{
	ASSERT(rows == cols, "Matrix determinant can only be found for square matrices");
	switch (rows) {
		case(1): { //a
			return data[0];
		}
		case(2): { //ad - bc
			return (data[0] * data[3]) - (data[1] * data[2]);
		}
		case(3): { //aei + bfg + cdh - ceg - bdi - afh
			return  (data[0] * data[4] * data[8]) +		//aei
					(data[1] * data[5] * data[6]) +		//bfg
					(data[2] * data[3] * data[7]) -		//cdh
					(data[2] * data[4] * data[6]) -		//ceg
					(data[1] * data[3] * data[8]) -		//bdi
					(data[0] * data[5] * data[7]);		//afh
		}
		case(4): { //not writing this out in letters
			return  data[ 0] * (data[ 5] * (data[10] * data[15] - data[11] * data[14]) -
								data[ 9] * (data[ 6] * data[15] - data[ 7] * data[14]) + 
								data[13] * (data[ 6] * data[11] - data[ 7] * data[10]))
													-
					data[ 4] * (data[ 1] * (data[10] * data[15] - data[11] * data[14]) -
								data[ 9] * (data[ 2] * data[15] - data[ 3] * data[14]) +
								data[13] * (data[ 2] * data[11] - data[ 3] * data[10]))
													+
					data[ 8] * (data[ 1] * (data[ 6] * data[15] - data[ 7] * data[14]) -
								data[ 5] * (data[ 2] * data[15] - data[ 3] * data[14]) +
								data[13] * (data[ 2] * data[ 7] - data[ 3] * data[ 6]))
													-
					data[12] * (data[ 1] * (data[ 6] * data[11] - data[ 7] * data[10]) -
								data[ 5] * (data[ 2] * data[11] - data[ 3] * data[10]) +
								data[ 9] * (data[ 2] * data[ 7] - data[ 3] * data[ 6]));
		}
		default: {
			float result = 0;
			for (int i = 0; i < cols; ++i) {
				result += data[i] * this->Cofactor(0, i);
			}
			return result;
		}
	}
}

//returns the transposed matrix of cofactors of this matrix
inline Matrix Matrix::Adjoint() const {
	ASSERT(rows == cols, "Matrix adjoint can only be found for square matrices");
	Matrix newMatrix(rows, cols);
	int index = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			newMatrix.data[index++] = this->Cofactor(i, j);
		}
	}
	return newMatrix.Transpose();
}

//returns the adjoint divided by the determinant
inline Matrix Matrix::Inverse() const {
	float determinant = this->Determinant();
	ASSERT(determinant, "Matrix inverse does not exist if determinant is zero");
	if (elementCount > 1) {
		return this->Adjoint() / determinant;
	}
	return Matrix(1, 1, {1.f / determinant});
}

//returns an identity matrix with the given dimensions
inline Matrix Matrix::Identity(uint32 rows, uint32 cols) {
	Matrix newMatrix(rows, cols);
	int index = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (i == j) {
				newMatrix.data[index++] = 1;
			} else {
				newMatrix.data[index++] = 0;
			}
		}
	}
	return newMatrix;
}