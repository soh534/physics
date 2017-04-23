inline const Real& Matrix3::operator()(int i, int j) const
{
	return m_data[i][j];
}

inline Real& Matrix3::operator()(int i, int j)
{
	return m_data[i][j];
}

inline void Matrix3::setIdentity()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == j)
			{
				m_data[i][j] = 1.f;
			}
			else
			{
				m_data[i][j] = 0.f;
			}
		}
	}
}

inline void Matrix3::setZero()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_data[i][j] = 0.f;
		}
	}
}

inline void Matrix3::setTranspose(const Matrix3& m)
{
	Assert(this != &m, "feeding self as input");

	for (unsigned int i = 0; i < 3; i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			(*this)(j, i) = m(i, j);
		}
	}
}

inline void Matrix3::setTranslationUnit(const Vector3& translation)
{
	m_data[0][2] = translation(0);
	m_data[1][2] = translation(1);
}

inline void Matrix3::setRotationUnit(const Real rotation)
{
	m_data[0][0] = cos(rotation); m_data[0][1] = -sin(rotation);
	m_data[1][0] = sin(rotation); m_data[1][1] = cos(rotation);
}

inline void Matrix3::setTranslation(const Vector3& translation)
{
	setIdentity();
	setTranslationUnit(translation);
}

inline void Matrix3::setRotation(const Real rotation)
{
	setIdentity();
	setRotationUnit(rotation);
}

inline void Matrix3::setTransform(const Vector3& translation, const Real rotation)
{
	setIdentity();
	setTranslationUnit(translation);
	setRotationUnit(rotation);
}

inline void Matrix3::setReflection(const Vector3& direction)
{
	setIdentity();
	const Real xsq = direction(0) * direction(0);
	const Real ysq = direction(1) * direction(1);
	m_data[0][0] = xsq - ysq;
	m_data[0][1] = 2.f * direction(0) * direction(1);
	m_data[1][0] = 2.f * direction(0) * direction(1);
	m_data[1][1] = ysq - xsq;
}

/// TO-DO - this function doesn't work, repair it
inline void Matrix3::setMul(const Matrix3& m0, const Matrix3& m1)
{
	Assert(this != &m0 && this != &m1 ,"Feeding self as input");
	setZero();
	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				m_data[i][j] += m0(i, k) * m1(k, j);
			}
		}
	}
}

inline void Matrix3::mul(const Matrix3& t)
{
	Assert(false ,"Function is not implemented yet");
}

inline void Matrix3::setAdd(const Matrix3& m0, const Matrix3& m1)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_data[i][j] = m0(i, j) + m1(i, j);
		}
	}
}

inline void Matrix3::setSub(const Matrix3& m0, const Matrix3& m1)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_data[i][j] = m0(i, j) - m1(i, j);
		}
	}
}

inline void Matrix3::setInverse22(const Matrix3& m)
{
	Assert(this != &m ,"Feeding self as input");
	setZero();
	Real invD = 1.f / (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0));
	m_data[0][0] = invD * m(1, 1);
	m_data[0][1] = -m(0, 1);
	m_data[1][0] = -m(1, 0);
	m_data[1][1] = invD * m(0, 0);
}

inline void Matrix3::setCrossMatrix(const Vector3& v)
{
	m_data[0][0] = 0.; m_data[0][1] = -v(2); m_data[0][2] = v(1);
	m_data[1][0] = v(2); m_data[1][1] = 0.; m_data[1][2] = -v(0);
	m_data[2][0] = -v(1); m_data[2][1] = v(0); m_data[2][2] = 0.;
}

inline void Matrix3::setDiagonalMatrix(const Vector3& d)
{
	setIdentity();
	m_data[0][0] = d(0);
	m_data[1][1] = d(1);
	m_data[2][2] = d(2);
}

inline void Matrix3::setDiagonalMatrix(const Real& r0, const Real& r1, const Real& r2)
{
	setIdentity();
	m_data[0][0] = r0;
	m_data[1][1] = r1;
	m_data[2][2] = r2;
}