#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <string>
#include <functional>
#include <ctime>
#include <windows.h>
using namespace std;

void FillContainer(vector<int>& c)
{
    for (int i = 0; i < 10; ++i)
    {
        c.push_back(i);
    }
}

template <int T>
void add10(int& val)
{
    val += T;
}

void add10(int& val)
{
    val +=11;
}

template <typename Container>
void PrintElements(Container c)
{
    typedef Container::value_type type; //获得类型

    cout << endl;

    ostream_iterator<type> os(cout, "\t");
    copy(c.begin(), c.end(), os);
}

struct CDHFunctor
{
    CDHFunctor(int val) : mVal(val){}

    void operator()(int& v)
    {
        v += mVal;
    }

private:
    int mVal;
};

int doubleGreaterQSort(const void* rv, const void* lv)
{
    return *(int*)rv > *(int*)lv;
}

inline bool doubleGreaterSort(const double& rv, const double& lv)
{
    return rv > lv;
}

struct Compare
{
    Compare(int val) : mVale(val){}

    bool operator()(int val)
    {
        return val > mVale;
    }
private:
    int mVale;
};

class Test
{
public:
    Test()
    {
        cout << i++ << endl;
    }
private:
    static int i;
};

int Test::i = 1;
int main()
{
   Test t[200];
}

// {
//     LONGLONG startTime = 0;
//     LONGLONG endTime = 0;
// 
//     srand((unsigned) time(NULL));
//     int doubleSort[length] = {0};
//     int doubleQSort[length] = {0};
//     int element = 0;
//     for (int i = 0; i < length; ++i)
//     {
//         element = rand()% 1000;
//         doubleSort[i] = element;
//         doubleQSort[i] = element;
//     }
// 
//     
// 
//     cout << "sort消耗的时间： " ;
//     QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
//     sort(doubleSort, doubleSort+length, greater<int>());
// 
// 
//     find_if(doubleSort, doubleSort + length, )
//     QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
//     cout << endTime - startTime << endl ;
// 
//     startTime = 0;
//     endTime = 0;
// 
//     cout << "qsort消耗的时间： " ;
//     QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
//     qsort(doubleQSort, length, sizeof(int), doubleGreaterQSort);
//     QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
//     cout << endTime - startTime <<endl;
// }


// 
//     vector<int>::const_iterator iterVec = find(vecSource.begin(), vecSource.end(), 111);
//     if (iterVec != vecSource.end())
//     {
//         cout << "found " << *iterVec << endl;
//     }
//     else
//     {
//         cout << "not found in vector" << endl;
//     }
//     set<int>::const_iterator iterSt = st.find(222);
//     if (iterSt != st.end())
//     {
//         cout << "found " << *iterSt << endl;
//     }
//     else
//     {
//         cout << "not found in set" << endl;
//     }
//}



struct baseFunctor
{
    int i;
    baseFunctor(int ii) : i(ii){}
    void operator()()
    {
        cout << i;
    }

};

struct DeriFunctor : public baseFunctor
{
    DeriFunctor(int  i) : baseFunctor(i){}
    void operator()()
    {
        cout << "From the derived" << i;
    }
};

template <typename T>
void PrintElement(T value)
{
    cout << value << "\t";
}
template <typename T>
T DoubleParameter(T value)
{
    return 2 * value;
}

template<typename Arg, typename Result>
struct func : public unary_function<Arg, Result>
{
    Result operator()(Arg arg)
    {
        cout << arg << "\t";

    }
};


//     vector<int> vec;
//     list<int> lst;
//     map<int,int> mp;
//     set<int> st;
// 
//     for (int i = 0; i < 10; ++i)
//     {
//         vec.push_back(i);
//         lst.push_back(i);
//         mp.insert(make_pair(i, i));
//         st.insert(i);
//     }
// 
//     cout << "vector: " << endl;
//     vector<int>::const_iterator iterVec(vec.begin());
//     while (iterVec != vec.end())
//     {
//         cout << *iterVec << endl;
//         ++iterVec;
//     }
// 
//     cout << "\nlist: " << endl;
//     list<int>::const_iterator iterLst(lst.begin());
//     while (iterLst != lst.end())
//     {
//         cout << *iterLst << endl;
//         ++iterLst;
//     }
// 
//     cout << "\nmap: " << endl;
//     map<int, int>::const_iterator iterMap(mp.begin());
//     while (iterMap != mp.end())
//     {
//         cout << "Key = " << iterMap->first
//              << "Value = " << iterMap->second
//              << endl;
//         ++iterMap;
//     }
// 
//     cout << "\nset: " << endl;
//     set<int>::const_iterator iterSet(st.begin());
//     while (iterSet != st.end())
//     {
//         cout << *iterSet << endl;
//         ++iterSet;
//     }
