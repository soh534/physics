#include <physicsViewer.h>
#include <physicsWorld.h>
#include <Renderer.h>

static void drawCircleShape( const physicsCircleShape* shape, const Vector4& position )
{
	drawCircle( position, shape->getRadius() );
}

static void drawBoxShape( const physicsBoxShape* shape, const Vector4& position, const Real rotation )
{
	const Vector4& he = shape->getHalfExtents();
	// TODO: Replace with multiplication by transformation matrix
	Vector4 posNe, posNw, posSe, posSw;
	posNe.setRotatedDir( he, rotation );
	posNe.setAdd( posNe, position );

	posNw.setNegate( he, 0 );
	posNw.setRotatedDir( posNw, rotation );
	posNw.setAdd( posNw, position );

	posSe.setNegate( he, 1 );
	posSe.setRotatedDir( posSe, rotation );
	posSe.setAdd( posSe, position );

	posSw.setNegate( he );
	posSw.setRotatedDir( posSw, rotation );
	posSw.setAdd( posSw, position );

	drawLine( posNe, posNw );
	drawLine( posNw, posSw );
	drawLine( posSw, posSe );
	drawLine( posSe, posNe );
}

static void drawConvexShape( const physicsConvexShape* shape, const Vector4& position, const Real rotation )
{
	const std::vector<Vector4> vertices = shape->getVertices();
	const std::vector<int>& connectivity = shape->getConnectivity();

	int numConnectivity = ( int ) connectivity.size();

	for ( int i = 1; i < numConnectivity; i++ )
	{
		Vector4 dirV0; dirV0.setRotatedDir( vertices[connectivity[i - 1]], rotation );
		Vector4 v0; v0.setAdd( position, dirV0 );

		Vector4 dirV1; dirV1.setRotatedDir( vertices[connectivity[i]], rotation );
		Vector4 v1; v1.setAdd( position, dirV1 );

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

static void drawBodyTransform( const physicsBody& body )
{
	// Add arrow marking position and orientation of body
	drawArrow( body.getPosition(), Vector4( 0.f, 10.f ).getRotatedDir( body.getRotation() ) );

	Vector4 offset( 5.f, 5.f );
	drawText( std::to_string( body.getBodyId() ), body.getPosition() + offset );
}

void physicsViewer::viewBroadphase( const physicsWorld* world )
{
	const std::vector<BroadphaseBody>& broadphaseBodies = world->getBroadphaseBodies();

	for ( auto iter = broadphaseBodies.begin(); iter != broadphaseBodies.end(); iter++ )
	{
		const physicsAabb& aabb = iter->aabb;
		drawBox( aabb.m_max, aabb.m_min, RED );
	}
}

void physicsViewer::viewShapes( const physicsWorld* world )
{
	const std::vector<BodyId>& activeBodyIds = world->getActiveBodyIds();

	for ( auto i = 0; i < activeBodyIds.size(); i++ )
	{
		int activeBodyId = activeBodyIds[i];
		const physicsBody& body = world->getBody( activeBodyId );
		const physicsShape* shape = body.getShape();

		// Add arrow marking position and orientation of body
		drawBodyTransform( body );

		// Draw the shapes itself
		switch ( shape->getType() )
		{
		case physicsShape::CIRCLE:
			drawCircleShape(
				static_cast< const physicsCircleShape* >( shape ),
				body.getPosition()
			);
			break;
		case physicsShape::BOX:
			drawBoxShape( static_cast< const physicsBoxShape* >( shape ),
				body.getPosition(), body.getRotation() 
			);
			break;
		case physicsConvexShape::CONVEX:
			drawConvexShape( static_cast< const physicsConvexShape* >( shape ),
				body.getPosition(), body.getRotation() 
			);
			break;
		}
	}
}