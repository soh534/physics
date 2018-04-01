#define _USE_MATH_DEFINES

#define Vector4_AllSetMask 0x8888

inline void Vector4::operator=( const Vector4& other )
{
	m_quad = other.m_quad;
}

inline void Vector4::set( const Real x, const Real y, const Real z, const Real w )
{
	m_quad = _mm_set_ps( w, z, y, x );
}

inline void Vector4::setAll( const Real x )
{
	m_quad = _mm_set_ps1( x );
}

inline void Vector4::setZero()
{
	m_quad = _mm_setzero_ps();
}

inline void Vector4::setAdd( const Vector4& a, const Vector4& b )
{
	m_quad = _mm_add_ps( a.m_quad, b.m_quad );
}

inline void Vector4::setSub( const Vector4& a, const Vector4& b )
{
	m_quad = _mm_sub_ps( a.m_quad, b.m_quad );
}

inline void Vector4::setMul( const Vector4& a, const Vector4& b )
{
	m_quad = _mm_mul_ps( a.m_quad, b.m_quad );
}

inline void Vector4::setMul( const Vector4& a, const Real s )
{
	Vector4 sv; sv.setAll( s );
	setMul( a, sv );
}

inline void Vector4::setAddMul( const Vector4& a, const Vector4& b, const Real s )
{
	Vector4 bs; bs.setMul( b, s );
	setAdd( a, bs );
}

inline void Vector4::setSubMul( const Vector4& a, const Vector4& b, const Real s )
{
	Vector4 bs; bs.setMul( b, s );
	setSub( a, bs );
}

inline void Vector4::setDiv( const Vector4& a, const Vector4& b )
{
	// TODO: Find better way to check for all four elements
	Assert( fabs( b( 0 ) ) > std::numeric_limits<Real>::min(), "Division by 0" );
	Assert( fabs( b( 1 ) ) > std::numeric_limits<Real>::min(), "Division by 0" );
	m_quad = _mm_div_ps( a.m_quad, b.m_quad );
}

inline void Vector4::setDiv( const Vector4& a, const Real s )
{
	Assert( fabs( s ) > std::numeric_limits<Real>::min(), "Division by 0" );
	Vector4 sv; sv.setAll( s );
	setDiv( a, sv );
}

template <int N>
inline void Vector4::setDot( const Vector4& a, const Vector4& b )
{
	Assert( N >= 1 && N <= 4, "Dotting invalid # of elements" );
	if ( N == 1 )      m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0x1F );
	else if ( N == 2 ) m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0x3F );
	else if ( N == 3 ) m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0x7F );
	else if ( N == 4 ) m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0xFF );
}

inline void Vector4::setDot3( const Vector4& a, const Vector4& b )
{
	setDot<3>( a, b );
}

inline void Vector4::setDot4( const Vector4& a, const Vector4& b )
{
	setDot<4>( a, b );
}

inline void Vector4::setCross( const Vector4& a, const Vector4& b )
{
	// Straight-forward implementation of
	// (a1 * b2 - a2 * b1) i
	// (a2 * b0 - a0 * b2) j
	// (a0 * b1 - a1 * b0) k

	m_quad = _mm_sub_ps(
		_mm_mul_ps( _mm_shuffle_ps( a.m_quad, a.m_quad, _MM_SHUFFLE( 3, 0, 2, 1 ) ), _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE( 3, 1, 0, 2 ) ) ),
		_mm_mul_ps( _mm_shuffle_ps( a.m_quad, a.m_quad, _MM_SHUFFLE( 3, 1, 0, 2 ) ), _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE( 3, 0, 2, 1 ) ) )
	);
}

inline void Vector4::setRotatedDir( const Vector4& v, const Real angle )
{
	Real xbuf = cos( angle )*v( 0 ) - sin( angle )*v( 1 );
	( *this )( 1 ) = sin( angle )*v( 0 ) + cos( angle )*v( 1 );
	( *this )( 0 ) = xbuf;
}

inline void Vector4::setRotatedDir( const Real angle )
{
	Real xbuf = cos( angle )*( *this )( 0 ) - sin( angle )*( *this )( 1 );
	( *this )( 1 ) = sin( angle )*( *this )( 0 ) + cos( angle )*( *this )( 1 );
	( *this )( 0 ) = xbuf;
}

template <int N>
inline void Vector4::setNormalized( const Vector4& a )
{
	Assert( !a.isZero(), "Trying to normalize a zero vector" );
	setDiv( a, a.length<N>() );
}

inline void Vector4::setNormalized3( const Vector4& a )
{
	setNormalized<3>( a );
}

inline void Vector4::setNormalized4( const Vector4& a )
{
	setNormalized<4>( a );
}

inline void Vector4::setNegate( const Vector4& a )
{
	// Reverse the sign-bit on the quads
	static const __m128 SIGNMASK = _mm_castsi128_ps( _mm_set1_epi32( 0x80000000 ) );
	m_quad = _mm_xor_ps( a.m_quad, SIGNMASK );
}

inline void Vector4::setNegate( const Vector4& v, unsigned int idx )
{
	// TODO: Account for all elements, find better way to negate single elements
	Assert( idx == 0 || idx == 1, "Trying to negate invalid index" );

	if ( idx == 0 )
	{
		( *this )( 0 ) = v( 0 ) * -1.0f;
		( *this )( 1 ) = v( 1 );
	}
	else
	{
		( *this )( 0 ) = v( 0 );
		( *this )( 1 ) = v( 1 ) * -1.0f;
	}
}

inline void Vector4::setMax( const Vector4 & v )
{
	m_quad = _mm_max_ps( m_quad, v.m_quad );
}

inline void Vector4::setMin( const Vector4 & v )
{
	m_quad = _mm_min_ps( m_quad, v.m_quad );
}

inline void Vector4::setInterpolate( const Vector4& a, const Vector4& b, const Real t )
{
	*this = a * ( 1.f - t ) + b * t;
}

// Getting
inline const Real& Vector4::operator()( int i ) const
{
	Assert( i >= 0 && i <= 3, "Looking up invalid index." );
	return m_quad.m128_f32[i];
}

inline Real& Vector4::operator()( int i )
{
	Assert( i >= 0 && i <= 3, "Looking up invalid index." );
	return m_quad.m128_f32[i];
}

inline const Vector4 Vector4::operator+( const Vector4& other ) const
{
	Vector4 res; res.setAdd( *this, other );
	return res;
}

inline void Vector4::operator+=( const Vector4& rhs )
{
	setAdd( *this, rhs );
}

inline const Vector4 Vector4::operator-( const Vector4& other ) const
{
	Vector4 res; res.setSub( *this, other );
	return res;
}

inline void Vector4::operator-=( const Vector4& rhs )
{
	setSub( *this, rhs );
}

inline const Vector4 Vector4::operator*( const Vector4& other ) const
{
	Vector4 res; res.setMul( *this, other );
	return res;
}

inline const Vector4 Vector4::operator*( const Real s ) const
{
	Vector4 res; res.setMul( *this, s );
	return res;
}

inline void Vector4::operator*=( const Vector4& rhs )
{
	setMul( *this, rhs );
}

inline void Vector4::operator*=( const Real s )
{
	setMul( *this, s );
}

inline const Vector4 Vector4::operator/( const Vector4& other ) const
{
	Vector4 res; res.setDiv( *this, other );
	return res;
}

inline const Vector4 Vector4::operator/( const Real s ) const
{
	Vector4 res; res.setDiv( *this, s );
	return res;
}

inline void Vector4::operator/=( const Vector4& rhs )
{
	setDiv( *this, rhs );
}

inline void Vector4::operator/=( const Real s )
{
	setDiv( *this, s );
}

template <int N>
inline Real Vector4::dot( const Vector4& other ) const
{
	Vector4 res; res.setDot<N>( *this, other );
	return res( 0 );
}

inline Real Vector4::dot3( const Vector4& other ) const
{
	Vector4 res; res.setDot3( *this, other );
	return res( 0 );
}

inline Real Vector4::dot4( const Vector4& other ) const
{
	Vector4 res; res.setDot4( *this, other );
	return res( 0 );
}

inline Vector4 Vector4::cross( const Vector4& other ) const
{
	Vector4 res; res.setCross( *this, other );
	return res;
}

template <int N>
inline Real Vector4::length() const
{
	return sqrt( dot<N>( *this ) );
}

inline Real Vector4::length3() const
{
	return sqrt( dot3( *this ) );
}

inline Real Vector4::length4() const
{
	return sqrt( dot4( *this ) );
}

template <int N>
inline Real Vector4::lengthSquared() const
{
	return dot<N>( *this );
}

inline Real Vector4::lengthSquared3() const
{
	return dot3( *this );
}

inline Real Vector4::lengthSquared4() const
{
	return dot4( *this );
}

inline Vector4 Vector4::getRotatedDir( const Real angle ) const
{
	Vector4 res; res.setRotatedDir( *this, angle );
	return res;
}

template <int N>
inline Vector4 Vector4::getNormalized() const
{
	Vector4 res; res.setNormalized<N>( *this );
	return res;
}

inline Vector4 Vector4::getNormalized3() const
{
	Vector4 res; res.setNormalized3( *this );
	return res;
}

inline Vector4 Vector4::getNormalized4() const
{
	Vector4 res; res.setNormalized4( *this );
	return res;
}

inline Vector4 Vector4::getNegated() const
{
	Vector4 res; res.setNegate( *this );
	return res;
}

inline Vector4 Vector4::getNegated( unsigned int idx ) const
{
	Assert( idx == 0 || idx == 1, "Trying to negate invalid index" );
	Vector4 res( *this );

	if ( idx == 0 )
	{
		res( 0 ) *= -1.0f;
	}
	else
	{
		res( 1 ) *= -1.0f;
	}

	return res;
}


inline Vector4 Vector4::getTripleCrossProduct( const Vector4& v1, const Vector4& v2 ) const
{
	// Implements (this x v1) x v2 
	Vector4 ab = this->cross( v1 );
	return ab.cross( v2 );
}

template <int N>
inline void Vector4::normalize()
{
	setNormalized<N>( *this );
}

inline void Vector4::normalize3()
{
	setNormalized3( *this );
}

inline void Vector4::normalize4()
{
	setNormalized4( *this );
}

inline void Vector4::negate()
{
	setNegate( *this );
}

// Comparison overloads
inline bool Vector4::operator > ( const Vector4& rhs ) const
{
	int mask = _mm_movemask_epi8( _mm_cvtps_epi32( _mm_cmplt_ps( m_quad, rhs.m_quad ) ) );
	return ( mask == 0 );
}

inline bool Vector4::operator > ( const Real rhs ) const
{
	int mask = _mm_movemask_epi8( _mm_cvtps_epi32( _mm_cmplt_ps( m_quad, _mm_set_ps1( rhs ) ) ) );
	return ( mask == 0 );
}

inline bool Vector4::operator < ( const Vector4& rhs ) const
{
	return !( *this > rhs );
}

inline bool Vector4::operator < ( const Real rhs ) const
{
	return !( *this > rhs );
}

inline bool Vector4::operator == ( const Vector4& rhs ) const
{
	int mask = _mm_movemask_epi8( _mm_cvtps_epi32( _mm_cmpeq_ps( m_quad, rhs.m_quad ) ) );
	return ( mask == Vector4_AllSetMask );
}

inline bool Vector4::operator == ( const Real rhs ) const
{
	int mask = _mm_movemask_epi8( _mm_cvtps_epi32( _mm_cmpeq_ps( m_quad, _mm_set_ps1( rhs ) ) ) );
	return ( mask == Vector4_AllSetMask );
}

inline bool Vector4::operator != ( const Vector4& rhs ) const
{
	return !( *this == rhs );
}

inline bool Vector4::operator != ( const Real rhs ) const
{
	return !( *this == rhs );
}

inline bool Vector4::isZero() const
{
	// TODO: return a static reference for zero vector
	Vector4 zero; zero.setZero();
	return ( *this == zero );
}

// TODO: potentially move to vector utility class
inline bool Vector4::isNan() const
{
	// TODO: more intelligent way
	return isnan( ( *this )( 0 ) ) || isnan( ( *this )( 1 ) ) || isnan( ( *this )( 2 ) );
}

inline bool Vector4::isInf() const
{
	return isinf( ( *this )( 0 ) ) || isinf( ( *this )( 1 ) ) || isinf( ( *this )( 2 ) );
}

inline bool Vector4::isOk() const
{
	return ( !isNan() && !isInf() );
}