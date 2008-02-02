#include <stdlib.h>
#include <iostream>

#include <UnitTest.hpp>

using namespace std;

int main(int argc, char ** argv)
{
	UnitTestManager testMngr;

    testMngr.run();
    testMngr.print();
    testMngr.save("report.txt");

    std::cin.get();

	return 0;
}
