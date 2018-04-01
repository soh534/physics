#pragma once

#include <ostream> // For ostream

class Vector4;

// General matrix class, used for constraint solver calculations
class Matrix
{
    Real* m_data;

    unsigned int m_rows;

    unsigned int m_cols;

    unsigned int m_size;

public:

    Matrix(unsigned int rows, unsigned int cols); // Constructor, initializes elements to 0

    Matrix(const Matrix& m);

    ~Matrix();

	inline unsigned int getRows() const;

	inline unsigned int getCols() const;

	inline unsigned int getSize() const;

	inline Real& operator()(unsigned int row, unsigned int col) const;

	// Set to identity
	inline void setIdentity();

	// Set all elements to f
	inline void set(const Real f);

	// Set into a scaling matrix from a single scalar (Square matrices only obviously)
	inline void setScale(const Real s);

	// Set into a scaling matrix from Vector3f (3x3 matrices only)
	inline void setScale(const Vector4& s);

	// Set into cross product in matrix form from Vector3f (3x3 matrices only)
	inline void setCross(const Vector4& v);

	// Set this matrix into product of m1 and m2
	inline void setMul(const Matrix& m1, const Matrix& m2);

	// Set this matrix into transpose of m
	inline void setTranspose(const Matrix& m);

	//
	// Utility functions
	friend std::ostream& operator << (std::ostream& os, const Matrix& m);
};

#include <Common/Matrix.inl>
