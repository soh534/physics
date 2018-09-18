#include <../Common/Vector4.h>
#include <../Common/ArrayFreeList.h>

class Shape
{


};

class Body
{
    Shape m_shape;

    Vector4 m_position;
};

class Constraint
{
    // Normal in body A space
    Vector4 m_normal;


};

class World
{
    void step()
    {
        // Collide
        // Find closest contact points, normalized normal

        // Naive SAT


        // Solve
        // Solve the constraint

    }

    ArrayFreeList<Vector4> m_positions;
    ArrayFreeList<Vector4> m_linearVelocities;

    ArrayFreeList<Vector4> m_orientations; // In quaternion form
    ArrayFreeList<Vector4> m_angularVelocites; // In (wx, wy, wz)

    // Inverse of diagonals of inertia in principal axes, 4th element inverse mass
    ArrayFreeList<Vector4> m_invInertiaMass; 
};
