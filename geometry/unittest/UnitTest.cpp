#include <UnitTest.hpp>

#include <stdarg.h>
#include <iostream>

vector<UnitTest*> * UnitTestManager::unitTestLst = 0;

UnitTest::UnitTest(const string & name)
: className(name)
{

}

void UnitTest::printf(const char * format, ...)
{
    va_list args;
    va_start(args, format);
#ifdef _WIN32
    vector<char> buffer(_vscprintf( format, args ) + 1);
#else
    vector<char> buffer(1024);
#endif
    vsprintf( &buffer[0], format, args );

    infos.push_back(className + std::string(": ") + std::string(&buffer[0]));

    va_end(args);    
}

UnitTestManager::UnitTestManager()
{
    
}

UnitTestManager::~UnitTestManager()
{

}

void UnitTestManager::print() const
{
    for(unsigned int i=0; i<unitTestLst->size(); i++){
        const vector<string> & infos = (*unitTestLst)[i]->getInfos();
        for(unsigned int i=0; i<infos.size(); i++){
            printf("%s\n", infos[i].c_str());
        }
    }
    //printf("Tests run: %d, Failures: %d Not run: %d, Time: .\n",unitTestLst->size());
    printf("Tests run: %d\n",(int)unitTestLst->size());
}

void UnitTestManager::save(const string & filename)
{
    FILE * fp = fopen(filename.c_str(), "wt");
    if(!fp) return;

    for(unsigned int i=0; i<unitTestLst->size(); i++){
        const vector<string> & infos = (*unitTestLst)[i]->getInfos();
        for(unsigned int i=0; i<infos.size(); i++){
            fprintf(fp, "%s\n", infos[i].c_str());
        }
    }
    fclose(fp);
}

void UnitTestManager::run()
{
    for(unsigned int i=0; i<unitTestLst->size(); i++){
        (*unitTestLst)[i]->run();
    }
}

int UnitTestManager::addTest(UnitTest* a_test)
{
    if(!unitTestLst) unitTestLst = new vector<UnitTest*>;

    unitTestLst->push_back(a_test);
    
    return (int)unitTestLst->size();
}
