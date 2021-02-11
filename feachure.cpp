#include <iostream>

class A;

typedef bool (A::*method_type)(const std::string &);

static void f(A * obj, method_type method) {
	if ((obj->*method)("Super message")) {
		std::cout << "Fuck yeah!" << std::endl;
	}
}

class A {
public:
	A() : _n(42) {}
	~A() {}

	void print() {
		std::cout << "A" << _n << std::endl;
	}
	void start() {
		f(this, &A::secret);
	}
private:
	bool secret(const std::string & text) {
		_n += text.size();
		std::cout << text << std::endl;
		return true;
	}
	std::string::size_type _n;
};


int main() {
	A	a;

	a.print();
	a.start();
	a.print();
}
