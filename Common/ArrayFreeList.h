#pragma once

template <typename T>
struct ArrayFreeList
{
    ArrayFreeList( int size = 128 )
    {
        // Must assert sizeof(T) > sizeof(int)

        m_elements = new T[size];
        m_nextFreeIds = new int[size];
        m_firstFreeId = 0;
        m_numElements = size;

        for ( int i = 0; i < m_numElements; i++ )
        {
            m_nextFreeIds[i] = i + 1;
        }
    }

    ~ArrayFreeList()
    {
        delete[] m_elements;
        delete[] m_nextFreeIds;
    }

    int getSize() { return m_numElements; }
    bool isValid( int index ) { return (m_nextFreeIds[index] == -1); }
    T& operator()( int index ) { return m_elements[index]; }

    int add( const T& t )
    {
        int thisId = m_firstFreeId;

        T& element = m_elements[thisId];
        element = t;

        m_firstFreeId = m_nextFreeIds[thisId];
        m_nextFreeIds[thisId] = -1;

        return thisId;
    }

    void remove( int index )
    {
        m_nextFreeIds[index] = m_firstFreeId;
        m_firstFreeId = index;
    }

private:

    T * m_elements;
    int* m_nextFreeIds;
    int m_firstFreeId;
    int m_numElements;
};
