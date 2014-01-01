#include <iostream>
#include <vector>
#include <map>
using namespace std;

template<typename inputIter>
typename iterator_traits<inputIter>::value_type Max(inputIter begin, inputIter end)
{
    typedef typename iterator_traits<inputIter>::value_type  type;
    type val = *begin;
    while (begin != end)
    {
        if (*begin > val)
        {
            val = *begin;
        }
        ++begin;
    }

    return val;
}

int main()
{
    vector<int> vec;
    for (int i = 0; i < 10; ++i)
    {
        vec.push_back(i);
    }

    vec.push_back(100);
    vec.push_back(1);
    map<int, int> mp;

    cout << Max(mp.begin(), mp.end()) << endl;
}