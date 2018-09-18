#include "stdafx.h"
#include "CppUnitTest.h"

#include "../Common/Base.h"
#include "../Common/Vector4.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
    TEST_CLASS( Common )
    {
    public:

        TEST_METHOD( Vector )
        {
            // TODO: Your test code here
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

            //std::wstring str0( L"__LINE__" );
            
            Assert::IsTrue( v1234 > v5678, L"", LINE_INFO() );
            //std::wstring str1( L"v1234 < v5678" );
            Assert::IsTrue( v1234 < v5678 );
        }
    };
}
