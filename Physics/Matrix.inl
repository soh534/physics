inline unsigned int Matrix::getRows() const
{
	return m_rows;
}

inline unsigned int Matrix::getCols() const
{
	return m_cols;
}

inline unsigned int Matrix::getSize() const
{
	return m_size;
}

inline Real& Matrix::operator()(unsigned int row, unsigned int col) const
{ // 0-Indexed
	Assert(0 <= row && row < m_rows, "illegal matrix row look-up");
	Assert(0 <= col && col < m_cols, "illegal matrix col look-up");
	unsigned int index = row * m_cols + col;
	Assert(index < m_rows*m_cols, "illegal matrix look-up");
	return m_data[index];
}

inline void Matrix::setIdentity()
{
	size_t numRows = getRows();
	size_t numCols = getCols();
	Assert(numRows == numCols, "Identity matrices must be square");

	memset(m_data, 0, sizeof(Real) * m_size);

	for (unsigned int i = 0; i < numRows; i++)
	{
		for (unsigned int j = 0; j < numCols; j++)
		{
			if (i==j)
			{
				(*this)(i, j) = 1.f;
			}
		}
	}
}

inline void Matrix::set(const Real f)
{
	for (unsigned int i = 0; i < getRows(); i++)
	{
		for (unsigned int j = 0; j < getCols(); j++)
		{
			(*this)(i, j) = f;
		}
	}
}

inline void Matrix::setScale(const Real s)
{
	setIdentity();
	(*this)(0, 0) = s;
	(*this)(1, 1) = s;
	(*this)(2, 2) = s;
}

inline void Matrix::setScale(const Vector4& s)
{
	Assert(getRows() == 3, "Scaling is only supported for 3-row matrices");
	setIdentity();
	(*this)(0, 0) = s(0);
	(*this)(1, 1) = s(1);
	(*this)(2, 2) = s(2);
}

inline void Matrix::setCross(const Vector4& v)
{
	Assert(getRows() == 3, "Cross is only supported for 3 by 3 matrices");
	Assert(getCols() == 3, "Cross is only supported for 3 by 3 matrices");
	(*this)(0, 0) = 0.f;   (*this)(0, 1) = -v(2); (*this)(0, 2) = v(1);
	(*this)(1, 0) = v(2);  (*this)(1, 1) = 0.f;   (*this)(1, 2) = -v(0);
	(*this)(2, 0) = -v(2); (*this)(2, 1) = v(0);  (*this)(2, 2) = 0.f;
}

inline void Matrix::setMul(const Matrix& m1, const Matrix& m2)
{
	Assert(m1.getCols() == m2.getRows(), "m1 column & m2 row mismatch");
	for (unsigned int i = 0; i < m1.getRows(); i++)
	{
		for (unsigned int j = 0; j < m2.getCols(); j++)
		{
			Real sum = 0.0f;
			
			for (unsigned int k = 0; k < m2.getRows(); k++)
			{
				sum += m1(i, k) * m2(k, j);
			}
			
			(*this)(i, j) = sum;
		}
	}
}

inline void Matrix::setTranspose(const Matrix& m)
{
	Assert(m_rows == m.getCols() && m_cols == m.getRows(), "dimension mismatch");
	
	for (unsigned int i = 0; i < m.getRows(); i++)
	{
		for (unsigned int j = 0; j < m.getCols(); j++)
		{
			(*this)(j, i) = m(i, j);
		}
	}
}