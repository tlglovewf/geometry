#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <vector>
#include <string>

using namespace std;

class UnitTest
{
public:
    UnitTest(const string & name);

    virtual void run() = 0;
    
    void printf(const char * cformat, ...);
    
    const vector<string> & getInfos(){ return infos; }

private:
    string className;
    
    vector<string> infos;
};

class UnitTestManager
{
public:
    UnitTestManager();
    virtual ~UnitTestManager();

    void run();
    void print() const;
    void save(const string & filename);
    
    static int addTest(UnitTest*);

    vector<UnitTest*> * getUnitTests(){ return unitTestLst; }

private:
    // warning: doesn't work if not ptr due to random static order init.
    static vector<UnitTest*> * unitTestLst;
};

#define ASSERT(exp) \
    if(exp){ \
    printf("assert at %s in %s:line %d",__FUNCTION__,__FILE__,__LINE__); \
    printf("in expr: %s", #exp); }

#define RUN_UNIT_TEST(className) \
    class className : public UnitTest{ \
        public: \
        className() : UnitTest(#className){} \
        void run(); \
        static const int init(){ return UnitTestManager::addTest(new className); } \
    };\
    static const int dummy_object##className = className::init(); \
    void className::run()

#endif