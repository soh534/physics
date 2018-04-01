#include <Common/Base.h>

//
// Matrixf class non-inline functions
//
Matrix::Matrix(unsigned int rows, unsigned int cols)
    : m_rows(rows), m_cols(cols)
{
    m_size = m_rows * m_cols;
    m_data = new Real[m_size];
    memset(m_data, 0, sizeof(Real) * m_size);
}

Matrix::Matrix(const Matrix& m)
    : m_rows(m.getRows()), m_cols(m.getCols())
{
    m_size = m_rows * m_cols;
    m_data = new Real [m_size];

    for (unsigned int i = 0; i < m_size; i++)
    {
        m_data[i] = m.m_data[i];
    }
}
    
Matrix::~Matrix()
{ // De-allocate allocated data
    delete[] m_data;
}

//
// Utility functions
//
std::ostream& operator << (std::ostream& os, const Matrix& m)
{
	os.precision(2);
	os << std::fixed;

    for (unsigned int i = 0; i < m.getRows(); i++)
    {
        for (unsigned int j = 0; j < m.getCols(); j++)
        {
			os << m(i, j);

			if (j < m.getCols() - 1)
			{
				os << "\t";
			}
        }
        os << std::endl;
    }

    return os;
}