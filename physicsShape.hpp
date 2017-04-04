#ifndef PHYSICS_SHAPE_HPP
#define PHYSICS_SHAPE_HPP

#include <Base.hpp>

#include <physicsObject.hpp>

#include <vector>

class physicsAabb;

//
// Base class for all shapes
//
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

    virtual void render(const Point3& pos, const Real rot) const = 0;

    virtual bool containsPoint(const Point3& point) const = 0;

    virtual void getSupportingVertex(const Vector3& direction, Point3& point) const = 0;

    virtual physicsAabb getAabb(const Real rot) const = 0;
};

//
// Circle shape
//
class physicsCircleShape : public physicsShape
{
  public:

    physicsCircleShape(Real radius);

    virtual ~physicsCircleShape() override;

	virtual Type getType() const override { return physicsShape::CIRCLE; }

	virtual Real calculateMass() const override;

	virtual Real calculateInertia() const override;
 
    virtual void render(const Point3& pos, const Real rot) const override;

    virtual bool containsPoint(const Point3& point) const override;

    virtual void getSupportingVertex(const Vector3& direction, Point3& point) const override;

    virtual physicsAabb getAabb(const Real rot) const override;

    inline Real getRadius() const { return m_radius; }

  protected:

    Real m_radius;
};

//
// Box shape
//
class physicsBoxShape : public physicsShape
{
  public:

    physicsBoxShape(const Vector3& halfExtents);

    virtual ~physicsBoxShape() override;

    virtual Type getType() const override { return physicsShape::BOX; }

	virtual Real calculateMass() const override;

	virtual Real calculateInertia() const override;

    virtual void render(const Point3& pos, const Real rot) const override;

    virtual bool containsPoint(const Point3& point) const override;

    virtual void getSupportingVertex(const Vector3& direction, Point3& point) const override;

    virtual physicsAabb getAabb(const Real rot) const override;

    inline const Vector3& getHalfExtents() const { return m_halfExtents; }
    
    // Determine edge facing point in this box's local coordinates
    void getEdgeFacingPoint(const Point3& point, Point3& base, Vector3& edge);
    
  protected:

    Vector3 m_halfExtents;
};

//
// Convex shape
//
class physicsConvexShape : public physicsShape
{
	static const unsigned int maxVertices = 20;

public:

	// Vertices passed can be unsorted
	// Vertices have to be w.r.t local-space
    physicsConvexShape(const std::vector<Point3>& vertices, const Real radius);

    virtual ~physicsConvexShape() override;

	virtual Type getType() const override { return physicsShape::CONVEX; }

	virtual Real calculateMass() const override;

	virtual Real calculateInertia() const override;

    virtual void render(const Point3& pos, const Real rot) const override;

    virtual bool containsPoint(const Point3& point) const override;

    virtual void getSupportingVertex(const Vector3& direction, Point3& point) const override;

    virtual physicsAabb getAabb(const Real rot) const override;

protected:

    std::vector<Point3> m_vertices;

    std::vector<int> m_connectivity;
};

#endif
