#include <memory>
#include <iostream>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace std;

struct A {
	unique_ptr<A> ptr;
	~A() {
		cout << "destruct";
	}
};

void f() {
	unique_ptr<A> a = make_unique<A>();
	a->ptr = std::move(a);
}

int main() {
	f();
	_CrtDumpMemoryLeaks();
}