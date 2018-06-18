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
    bool isUsed( int index ) { return (m_nextFreeIds[index] == -1); }
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


struct Strukt
{
    enum
    {
        INVALID = -1
    };

    int content;
    int nextFreeStruktId;
};

struct StruktManager
{
    enum 
    {
        SZ_STRUKTS = 100
    };

    Strukt m_strukts[SZ_STRUKTS];

    StruktManager()
    {
        m_nextFreeStruktId = 0;

        for ( int i = 0; i < SZ_STRUKTS; i++ )
        {
            Strukt& strukt = getStrukt( i );

            strukt.content = Strukt::INVALID;

            if ( i == SZ_STRUKTS - 1 )
            {
                strukt.nextFreeStruktId = Strukt::INVALID;
            }
            else
            {
                strukt.nextFreeStruktId = i + 1;
            }
        }
    }

    Strukt& getStrukt( int index )
    {
        return m_strukts[index];
    }

    int add( int i )
    {
        Strukt& strukt = getStrukt( m_nextFreeStruktId );

        // Update next free strukt, update memory stuff
        int disIndex = m_nextFreeStruktId;
        m_nextFreeStruktId = strukt.nextFreeStruktId;
        strukt.nextFreeStruktId = Strukt::INVALID;

        // Fill content
        strukt.content = i;

        return disIndex;
    }

    void remove( int index )
    {
        Strukt& strukt = getStrukt( index );

        // Add this strukt in front of free list
        strukt.content = Strukt::INVALID;
        strukt.nextFreeStruktId = m_nextFreeStruktId;
        m_nextFreeStruktId = index;
    }

    int m_nextFreeStruktId;
};

int arrayFreeListTest()
{
    StruktManager sm;

    for ( int i = 0; i < 100; i++ )
    {
        sm.add( i );
    }

    sm.remove( 49 );
    sm.remove( 64 );
    int idForThirtyTwo = sm.add( 32 );

    ArrayFreeList<int> arrayFreeList;

    for ( int i = 0; i < arrayFreeList.getSize(); i++ )
    {
        arrayFreeList.add( i );
    }

    arrayFreeList.remove( 49 );
    arrayFreeList.remove( 64 );
    idForThirtyTwo = arrayFreeList.add( 32 );

    return 0;
}