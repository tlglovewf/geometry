#include <UnitTest.hpp>
#include <gtl/matrix4.hpp>

using namespace gtl;

RUN_UNIT_TEST(TestMatrix4)
{
    Matrix4f mat4f1(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);

    ASSERT(!mat4f1.isIdentity());

    Matrix4f mat4f2;
    mat4f2.makeIdentity();

    ASSERT(mat4f1 != mat4f2);
    ASSERT(!mat4f1.equals(mat4f2));
}