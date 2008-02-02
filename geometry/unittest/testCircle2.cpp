#include <UnitTest.hpp>
#include <geometry/Circle2.hpp>

using namespace geo;

RUN_UNIT_TEST(TestCircle2)
{
    Circle2f c1(Vec2f(-1.0f,-1.0f), 1.5f);
    Circle2f c2(Vec2f( 1.0f, 1.0f), 2.0f);

    Vec2f p1,p2;

    if( c1.intersect(c2, p1, p2) ){
        cout << "p1: " << p1 << " (-0.93614  0.49864)" << endl;
        cout << "p2: " << p2 << " ( 0.49864 -0.93614)" << endl;
    }

	c1.getSurface();
	c1.getCircumference();
}

