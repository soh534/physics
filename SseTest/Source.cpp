#include <nmmintrin.h>

class Vector4
{
public:

	void set( const float x, const float y, const float z, const float w = 0.f )
	{
		m_quad = _mm_setr_ps( x, y, z, w );
	}

	void setAdd( const Vector4& a, const Vector4& b )
	{
		m_quad = _mm_add_ps( a.m_quad, b.m_quad );
	}

	void setSub( const Vector4& a, const Vector4& b )
	{
		m_quad = _mm_sub_ps( a.m_quad, b.m_quad );
	}

	void setMul( const Vector4& a, const Vector4& b )
	{
		m_quad = _mm_mul_ps( a.m_quad, b.m_quad );
	}

	void setDiv( const Vector4& a, const Vector4& b )
	{
		m_quad = _mm_div_ps( a.m_quad, b.m_quad );
	}

	template <int N>
	inline void setDot( const Vector4& a, const Vector4& b )
	{
		if ( N == 1 )      m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0x1F );
		else if ( N == 2 ) m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0x3F );
		else if ( N == 3 ) m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0x7F );
		else if ( N == 4 ) m_quad = _mm_dp_ps( a.m_quad, b.m_quad, 0xFF );
	}

	void setCross( const Vector4& a, const Vector4& b )
	{
		m_quad = _mm_sub_ps(
			_mm_mul_ps( _mm_shuffle_ps( a.m_quad, a.m_quad, _MM_SHUFFLE( 3, 0, 2, 1 ) ), _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE( 3, 1, 0, 2 ) ) ),
			_mm_mul_ps( _mm_shuffle_ps( a.m_quad, a.m_quad, _MM_SHUFFLE( 3, 1, 0, 2 ) ), _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE( 3, 0, 2, 1 ) ) )
			);
	}

	void setNegate( const Vector4& a )
	{
		static const __m128 SIGNMASK = _mm_castsi128_ps( _mm_set1_epi32( 0x80000000 ) );
		m_quad = _mm_xor_ps( a.m_quad, SIGNMASK );
	}

	bool operator > ( const Vector4& rhs ) const
	{
		int mask = _mm_movemask_epi8( _mm_cvtps_epi32( _mm_cmplt_ps( m_quad, rhs.m_quad ) ) );
		return ( mask == 0 );
	}

	bool operator < ( const Vector4& rhs ) const
	{
		return !( *this > rhs );
	}

	bool operator == ( const Vector4& rhs ) const
	{
		int mask = _mm_movemask_epi8( _mm_cvtps_epi32( _mm_cmpeq_ps( m_quad, rhs.m_quad ) ) );
		return ( mask == 0x8888 );
	}

	bool operator != ( const Vector4& rhs ) const
	{
		return !( *this == rhs );
	}


	__m128 m_quad;
};

int main( int argc, char* argv[] )
{
	Vector4 v1234; v1234.set( 1, 2, 3, 4 );
	Vector4 v5678; v5678.set( 5, 6, 7, 8 );
	Vector4 v1278; v1278.set( 1, 2, 7, 8 );
	Vector4 v5634; v5634.set( 5, 6, 3, 4 );

	Vector4 vadd; vadd.setAdd( v1234, v5678 );
	Vector4 vsub; vsub.setSub( v1234, v5678 );
	Vector4 vmul; vmul.setMul( v1234, v5678 );
	Vector4 vdiv; vdiv.setDiv( v1234, v5678 );
	Vector4 vcrs; vcrs.setCross( v1234, v5678 );
	Vector4 vneg; vneg.setNegate( v1234 );
	Vector4 vdot3; vdot3.setDot<3>( v1234, v5678 );
	Vector4 vdot4; vdot4.setDot<4>( v1234, v5678 );
	Vector4 vneg0; vneg0.setNegate( v1234 );
	bool gt = v1234 > v5678;
	bool lt = v1234 < v5678;
	bool gtt = v5678 > v1234;
	bool ltt = v5678 < v1234;
	bool eq = v1234 == v5678;
	bool eqq = v1234 != v5678;
	bool eqqq = v1234 == v1234;
	bool eqqqq = v5678 == v5678;
	bool eqqqqq = v1278 != v1234;

	return 0;
}