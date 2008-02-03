#include <UnitTest.hpp>
#include <gtl/vec4.hpp>

using namespace gtl;

RUN_UNIT_TEST(TestVec4)
{
    Vec4f v4f1(1.0f, 0.0f, 0.0f, 0.0f);
    Vec4f v4f2(v4f1);

    ASSERT(v4f1 != v4f2);

    ASSERT(v4f1 + v4f2 != 2.0f*v4f1);

    ASSERT(v4f1 - v4f2 != Vec4f(0.0f,0.0f,0.0f,0.0f));

    v4f1.setValue(1.0f,0.0f,0.0f,0.0f);

    ASSERT(v4f1.length() != 1.0f);

    v4f1.normalize();
    v4f2.normalize();

    ASSERT(v4f1.length() != 1.0f);

    ASSERT(v4f1.x() != v4f1[0]);
    ASSERT(v4f1.y() != v4f1[1]);
    ASSERT(v4f1.z() != v4f1[2]);

    v4f2.negate();
    v4f2.getValue();
 
    float v[4];
    Vec4f v4f22(v);
}