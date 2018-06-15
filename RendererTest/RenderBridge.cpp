#include "RenderBridge.h"

/*
void Renderer::drawCross( const Vector4& pos, const Real rot, const Real len, unsigned int color )
{
	Vector4 needle( len / 2.f, 0.f );

	Vector4 a, b, c, d;

	a = pos + needle.getRotatedDir( rot );
	b = pos + needle.getRotatedDir( rot + 90.f * g_degToRad );
	c = pos + needle.getRotatedDir( rot + 180.f * g_degToRad );
	d = pos + needle.getRotatedDir( rot + 270.f * g_degToRad );

	drawLine( a, c, color );
	drawLine( b, d, color );
}

void Renderer::drawArrow( const Vector4& pos, const Vector4& dir, unsigned int color )
{
	Vector4 dst; dst.setAdd( pos, dir );
	drawLine( pos, dst, color );

	Vector4 dirFrac = dir * .5f;
	Vector4 headLeft = dirFrac.getRotatedDir( 135.f * g_degToRad );
	Vector4 headRight = dirFrac.getRotatedDir( -135.f * g_degToRad );
	Vector4 dstToHeadLeft = dst + headLeft;
	Vector4 dstToHeadRight = dst + headRight;

	drawLine( dst, dstToHeadLeft, color );
	drawLine( dst, dstToHeadRight, color );
}

void Renderer::drawBox( const Vector4& max, const Vector4& min, unsigned int color )
{
	Vector4 upperLeft( min( 0 ), max( 1 ) );
	Vector4 bottomRight( max( 0 ), min( 1 ) );

	drawLine( max, upperLeft, color );
	drawLine( max, bottomRight, color );
	drawLine( min, upperLeft, color );
	drawLine( min, bottomRight, color );
}

void Renderer::drawCircle( const Vector4& pos, const Real radius, unsigned int color )
{
	Real step = 2 * (Real)M_PI * STEP_RENDER_CIRCLE;
	Real full = (2.f + STEP_RENDER_CIRCLE) * M_PI;

	for ( Real i = step; i < full; i += step )
	{
		Vector4 na, nb;
		na.set( radius * cos( i ), radius * sin( i ) );
		nb.set( radius * cos( i + step ), radius * sin( i + step ) );
		drawLine( pos + na, pos + nb );
	}
}

void Renderer::drawTriangle( const Vector4& a, const Vector4& b, const Vector4& c, unsigned int color )
{
	m_displayTris.push_back( DisplayTriangle( a, b, c, color ) );
}

void Renderer::drawBox( const Box& box, unsigned int color )
{

}

void Renderer::drawText( const std::string& str, const Vector4& pos, const Real scale, unsigned int color )
{
	m_displayTexts.push_back( DisplayText( str, pos, scale, color ) );
}
*/