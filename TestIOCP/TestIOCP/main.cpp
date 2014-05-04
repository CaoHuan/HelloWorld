
#include <iostream>
#include "WrapData.h"
#include <cstddef>
using namespace std;

template<class T>
struct OverlappedWrapper : T
{
public:
	int i;
};

class Test
{
public:

	int b;
};

int main()
{
	OverlappedWrapper<Test> a;
	a.b = 1;
	 cout << a.b << endl;
	return 0;
}
