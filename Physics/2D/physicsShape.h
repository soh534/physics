#pragma once

#include <memory>
#include <vector>
#include <Base.h>
#include <physicsObject.h>

class physicsAabb;

// Base class for all shapes
class physicsShape : public physicsObject
{
public:

    enum Type
    {
        BASE = 0,
        CIRCLE,
        BOX,
        CONVEX,
		NUM_SHAPES
    };
	
	Real m_convexRadius;

public:

    physicsShape();

	virtual ~physicsShape();

    virtual Type getType() const { return physicsShape::BASE; }

	virtual Real calculateMass() const = 0;

	virtual Real calculateInertia() const = 0;

    virtual bool containsPoint(const Vector4& point) const = 0;

    virtual void getSupportingVertex(const Vector4& direction, Vector4& point) const = 0;

    virtual physicsAabb getAabb(const Real rot) const = 0;
};

// Circle shape
class physicsCircleShape : public physicsShape
{
public:

	static std::shared_ptr<physicsShape> create( const Real radius );

    virtual ~physicsCircleShape() override;

	virtual Type getType() const override { return physicsShape::CIRCLE; }

	virtual Real calculateMass() const override;

	virtual Real calculateInertia() const override;
 
    virtual bool containsPoint(const Vector4& point) const override;

    virtual void getSupportingVertex(const Vector4& direction, Vector4& point) const override;

    virtual physicsAabb getAabb(const Real rot) const override;

    Real getRadius() const { return m_radius; }

protected:

	physicsCircleShape( const Real radius );

    Real m_radius;
};

// Box shape
class physicsBoxShape : public physicsShape
{
public:

	static std::shared_ptr<physicsShape> create( const Vector4& halfExtents );

    virtual ~physicsBoxShape() override;

    virtual Type getType() const override { return physicsShape::BOX; }

	virtual Real calculateMass() const override;

	virtual Real calculateInertia() const override;

	virtual bool containsPoint( const Vector4& point ) const override;

	virtual void getSupportingVertex( const Vector4& direction, Vector4& point ) const override;

	virtual physicsAabb getAabb( const Real rot ) const override;

	const Vector4& getHalfExtents() const { return m_halfExtents; }
    
protected:

    physicsBoxShape(const Vector4& halfExtents);

    Vector4 m_halfExtents;
};

// Convex shape
class physicsConvexShape : public physicsShape
{
public:

	static std::shared_ptr<physicsShape> create( const std::vector<Vector4>& vertices, const Real radius );

    virtual ~physicsConvexShape() override;

	virtual Type getType() const override { return physicsShape::CONVEX; }

	virtual Real calculateMass() const override;

	virtual Real calculateInertia() const override;

    virtual bool containsPoint(const Vector4& point) const override;

    virtual void getSupportingVertex(const Vector4& direction, Vector4& point) const override;

    virtual physicsAabb getAabb(const Real rot) const override;

	bool getAdjacentVertices( const Vector4& vertex, Vector4& va, Vector4& vb );

	const std::vector<Vector4>& getVertices() const { return m_vertices; }

	const std::vector<int>& getConnectivity() const { return m_connectivity; }

public:

	static const unsigned int maxVertices = 20;

protected:

	// Vertices passed can be unsorted
	// Vertices have to be w.r.t local-space
	physicsConvexShape( const std::vector<Vector4>& vertices, const Real radius );

    std::vector<Vector4> m_vertices;

    std::vector<int> m_connectivity; // Wraps towards the end
};
