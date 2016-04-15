# Introduction #

This page provides everything you'll need to know about getting started with the geometry library.

# A simple program using GTL #

```
#include <iostream>

#include <gtl/vec3.hpp>

using namespace gtl;

int main(int argc, char** argv)
{
    Vec3<float> v(1.0f, 0.0f, 0.0f);

    std::cout << "Length " << v.lenght() << std::endl;

    return 0;
}
```