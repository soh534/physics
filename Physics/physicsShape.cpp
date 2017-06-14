#include <physicsShape.h>
#include <physicsAabb.h>

#include <vector>
#include <climits>
#include <bitset> // For flags during convex vertices ordering
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Renderer.h>

const Real g_density = .01f;

///#define D_PRINT_VERTICES
#if defined (D_PRINT_VERTICES)
#include <sstream>
#endif

/// Base shape class functions
physicsShape::physicsShape()
	: m_convexRadius( 0.1f )
{

}

physicsShape::~physicsShape()
{

}

/// Circle shape class functions
std::shared_ptr<physicsShape> physicsCircleShape::create( const Real radius )
{
	return std::shared_ptr<physicsShape>( new physicsCircleShape( radius ) );
}

physicsCircleShape::physicsCircleShape( const Real radius )
	: m_radius( radius )
{

}

physicsCircleShape::~physicsCircleShape()
{

}

Real physicsCircleShape::calculateMass() const
{
	Real area = ( Real )M_PI * m_radius * m_radius;
	return ( g_density * area );
}

Real physicsCircleShape::calculateInertia() const
{
#if 1
	/// TODO: For now use aabb shape
	Real a = m_radius;
	Real b = m_radius;

	return ( ( 4.f / 3.f ) * g_density * ( a*a*a*b + a*b*b*b ) );
#else
	return calculateMass();
#endif
}

void physicsCircleShape::render( const Vector3& pos, const Real rot ) const
{
	/// TODO: move this out of API
	drawCircle( pos, m_radius );
}

bool physicsCircleShape::containsPoint( const Vector3& point ) const
{
	Real distSq = point.lengthSquared();

	if ( distSq < m_radius*m_radius )
	{
		return true;
	}

	return false;
}

void physicsCircleShape::getSupportingVertex( const Vector3& direction, Vector3& point ) const
{
	point.setMul( direction, m_radius );
}

physicsAabb physicsCircleShape::getAabb( const Real rot ) const
{
	Vector3 halfExtent; halfExtent.setAll( m_radius );
	Vector3 halfExtentNeg; halfExtentNeg.setNegated( halfExtent );
	return physicsAabb( halfExtent, halfExtentNeg );
}

std::shared_ptr<physicsShape> physicsBoxShape::create( const Vector3 & halfExtents )
{
	return std::shared_ptr<physicsShape>( new physicsBoxShape( halfExtents ) );
}

/// Box shape class functions
physicsBoxShape::physicsBoxShape( const Vector3& halfExtents )
	: m_halfExtents( halfExtents )
{

}

physicsBoxShape::~physicsBoxShape()
{

}

Real physicsBoxShape::calculateMass() const
{
	return ( 4.f * m_halfExtents( 0 ) * m_halfExtents( 1 ) * g_density );
}

Real physicsBoxShape::calculateInertia() const
{
	/// From gray book: I = 4/3 p (aaab + abbb)
#if 1
	Real a = m_halfExtents( 0 );
	Real b = m_halfExtents( 1 );
	return ( ( 4.f / 3.f ) * g_density * ( a*a*a*b + a*b*b*b ) );
#else
	return calculateMass();
#endif
}

void physicsBoxShape::render( const Vector3& pos, const Real rot ) const
{
	/// Replace with multiplication by transformation matrix
	Vector3 posNe, posNw, posSe, posSw;
	posNe.setRotatedDir( m_halfExtents, rot );
	posNe.setAdd( posNe, pos );

	posNw.setNegated( m_halfExtents, 0 );
	posNw.setRotatedDir( posNw, rot );
	posNw.setAdd( posNw, pos );

	posSe.setNegated( m_halfExtents, 1 );
	posSe.setRotatedDir( posSe, rot );
	posSe.setAdd( posSe, pos );

	posSw.setNegated( m_halfExtents );
	posSw.setRotatedDir( posSw, rot );
	posSw.setAdd( posSw, pos );

	drawLine( posNe, posNw );
	drawLine( posNw, posSw );
	drawLine( posSw, posSe );
	drawLine( posSe, posNe );
}

bool physicsBoxShape::containsPoint( const Vector3& point ) const
{
	if ( -1.0f * m_halfExtents( 0 ) <= point( 0 ) && point( 0 ) <= m_halfExtents( 0 ) &&
		 -1.0f * m_halfExtents( 0 ) <= point( 1 ) && point( 1 ) <= m_halfExtents( 1 ) )
	{
		return true;
	}

	return false;
}

void physicsBoxShape::getSupportingVertex( const Vector3& direction, Vector3& point ) const
{
	Vector3 dirNw( -m_halfExtents( 0 ), m_halfExtents( 1 ) );
	Vector3 dirSw( -m_halfExtents( 0 ), -m_halfExtents( 1 ) );
	Vector3 dirSe( m_halfExtents( 0 ), -m_halfExtents( 1 ) );
	Vector3 dirNe( m_halfExtents( 0 ), m_halfExtents( 1 ) );

	Real currMax = std::numeric_limits<Real>::lowest();
	Real potentialMaxDot;

	potentialMaxDot = direction.dot( dirNw );
	if ( potentialMaxDot > currMax )
	{
		currMax = potentialMaxDot;
		point = dirNw;
	}

	potentialMaxDot = direction.dot( dirSw );
	if ( potentialMaxDot > currMax )
	{
		currMax = potentialMaxDot;
		point = dirSw;
	}

	potentialMaxDot = direction.dot( dirSe );
	if ( potentialMaxDot > currMax )
	{
		currMax = potentialMaxDot;
		point = dirSe;
	}

	potentialMaxDot = direction.dot( dirNe );
	if ( potentialMaxDot > currMax )
	{
		currMax = potentialMaxDot;
		point = dirNe;
	}
}

physicsAabb physicsBoxShape::getAabb( const Real rot ) const
{
	/// ERROR: this shouldn't have to convert to radians
	Real radians = rot * ( Real )M_PI / 180.0f;
	Real w = 2.0f * m_halfExtents( 0 ) * cos( radians ) + 2.0f * m_halfExtents( 1 ) * sin( radians );
	Real h = 2.0f * m_halfExtents( 0 ) * sin( radians ) + 2.0f * m_halfExtents( 1 ) * cos( radians );

	return physicsAabb(
		Vector3( w / 2.0f, h / 2.0f ),
		Vector3( -w / 2.0f, -h / 2.0f ) );
}

/// Convex shape class functions
std::shared_ptr<physicsShape> physicsConvexShape::create( const std::vector<Vector3>& vertices, const Real radius )
{
	return std::shared_ptr<physicsShape>( new physicsConvexShape( vertices, radius ) );
}

physicsConvexShape::physicsConvexShape( const std::vector<Vector3>& vertices, const Real radius )
{ 
	/// Get unsorted list of vertices, establish connections to treat as convex

	/// TODO: Bug where code will fail if two same vertices exist in vertices array
	int numVertices = ( int )vertices.size();

	/// TODO: APPLY CONVEX RADIUS
	m_vertices.assign( vertices.begin(), vertices.end() );

	/// Determine connectivity
	unsigned int xMinIdx = 0;

	for ( int i = 1; i < numVertices; i++ )
	{
		if ( m_vertices[xMinIdx]( 0 ) > m_vertices[i]( 0 ) )
		{
			xMinIdx = i;
		}
	}

	m_connectivity.push_back( xMinIdx );

	int nodeCurrent = xMinIdx;
	int nodeNext = -1;

	Vector3 edgeCurrent;
	edgeCurrent.set( 0.0f, 1.0f );

	std::vector<bool> flags;
	flags.resize( numVertices, false );

	for ( int i = 0; i < numVertices; i++ )
	{
		Real cosCurrent = -1.0f;

		for ( int j = 0; j < numVertices; j++ )
		{
			if ( j == nodeCurrent || flags[j] )
			{
				continue;
			}

			Vector3 edgePotential;
			edgePotential.setSub( m_vertices[j], m_vertices[nodeCurrent] );

			Real cosPotential = edgeCurrent.dot( edgePotential ) / ( edgeCurrent.length() * edgePotential.length() );

			if ( cosPotential > cosCurrent )
			{
				nodeNext = j;
				cosCurrent = cosPotential;
			}
		}

		flags[nodeNext] = true;
		m_connectivity.push_back( nodeNext );
		edgeCurrent.setSub( m_vertices[nodeNext], m_vertices[nodeCurrent] );
		nodeCurrent = nodeNext;
	}
}

physicsConvexShape::~physicsConvexShape()
{

}

Real physicsConvexShape::calculateMass() const
{
	Real area = 0.f;

	const Vector3& v0 = m_vertices[m_connectivity[0]];

	for ( int i = 1; i < ( int )m_vertices.size(); i++ )
	{
		area += v0.cross( m_vertices[m_connectivity[1]] )( 2 );
	}

	return g_density * -1.f * area;
}

Real physicsConvexShape::calculateInertia() const
{
#if 1
	/// TODO: For now use aabb shape
	physicsAabb aabb = getAabb( 0.f );

	Real a = aabb.m_max( 0 );
	Real b = aabb.m_max( 1 );

	return calculateMass() * ( 4 * a*a + 4 * b*b ) / 12.f;
#else
	return calculateMass();
#endif
}

void physicsConvexShape::render( const Vector3& pos, const Real rot ) const
{
	int numConnectivity = ( int )m_connectivity.size();

	for ( int i = 1; i < numConnectivity; i++ )
	{
		Vector3 dirV0; dirV0.setRotatedDir( m_vertices[m_connectivity[i - 1]], rot );
		Vector3 v0; v0.setAdd( pos, dirV0 );

		Vector3 dirV1; dirV1.setRotatedDir( m_vertices[m_connectivity[i]], rot );
		Vector3 v1; v1.setAdd( pos, dirV1 );

		drawLine( v0, v1 );

#if defined (D_PRINT_VERTICES)
		std::stringstream ss;
		ss << v0;
		drawText( ss.str(), v0 );

		ss.str( "" );

		ss << v1;
		drawText( ss.str(), v1 );
#endif
	}
}

bool physicsConvexShape::containsPoint( const Vector3& point ) const
{
	Vector3 edge, normal, dirMpToPoint;

	int numConnectivity = ( int )m_connectivity.size();

	for ( int i = 0; i < numConnectivity - 1; i++ )
	{
		edge.setSub( m_vertices[m_connectivity[i + 1]], m_vertices[m_connectivity[i]] );
		normal.set( edge( 1 ), -edge( 0 ) );

		Vector3 midpoint; midpoint.setAddMul( m_vertices[m_connectivity[i]], edge, 0.5f );
		dirMpToPoint.setSub( point, midpoint );

		if ( normal.dot( dirMpToPoint ) < 0.0f )
		{
			return false;
		}
	}

	return true;
}

void physicsConvexShape::getSupportingVertex( const Vector3& direction, Vector3& point ) const
{
	Real dotMax = std::numeric_limits<Real>::lowest();
	Real potentialMaxDot;

	int numVertices = ( int )m_vertices.size();

	for ( int i = 0; i < numVertices; i++ )
	{
		potentialMaxDot = direction.dot( m_vertices[i] );

		if ( potentialMaxDot > dotMax )
		{
			dotMax = potentialMaxDot;
			point = m_vertices[i];
		}
	}
}

physicsAabb physicsConvexShape::getAabb( const Real rot ) const
{
	Real xmin, xmax, ymin, ymax;

	xmax = ymax = std::numeric_limits<Real>::lowest();
	xmin = ymin = std::numeric_limits<Real>::max();

	int numVertices = ( int )m_vertices.size();
	for ( int i = 0; i < numVertices; i++ )
	{
		Vector3 vertW = m_vertices[i].getRotatedDir( rot );
		xmin = std::min( vertW( 0 ), xmin );
		xmax = std::max( vertW( 0 ), xmax );
		ymin = std::min( vertW( 1 ), ymin );
		ymax = std::max( vertW( 1 ), ymax );
	}

	return physicsAabb( Vector3( xmax, ymax ), Vector3( xmin, ymin ) );
}

bool physicsConvexShape::getAdjacentVertices( const Vector3& vertex, Vector3& va, Vector3& vb )
{
	int numVertices = ( int )m_vertices.size();
	int idx = -1;

	for ( int i = 0; i < numVertices; i++ )
	{
		/// TODO: measure appropriate epsilon and make it const var
		if ( (vertex - m_vertices[i]).lengthSquared() < 0.01f )
		{
			idx = i;
			break;
		}
	}

	if ( idx < 0 )
	{
		return false;
	}

	int numConnectivity = ( int )m_connectivity.size();
	int idxConnection = -1;

	for ( int i = 0; i < numConnectivity; i++ )
	{
		if ( idx == m_connectivity[i] )
		{
			idxConnection = i;
			break;
		}
	}

	Assert( idxConnection > -1, "Connectivity data in convex shape doesn't match vertices index." );

	/// TODO: so messy, clean up
	int idxVertA = ( idxConnection == 0 ) ? m_connectivity[numConnectivity - 2] : m_connectivity[idxConnection - 1];
	int idxVertB = (idxConnection == numConnectivity - 1) ? m_connectivity[1] : m_connectivity[idxConnection + 1];

	va = m_vertices[idxVertA];
	vb = m_vertices[idxVertB];

	return true;
}
