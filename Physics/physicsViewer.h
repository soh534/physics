#pragma once

#include <memory>

class physicsWorld;

namespace physicsViewer
{
	void viewBroadphase( const physicsWorld* world );

	void viewShapes( const physicsWorld* world );
}