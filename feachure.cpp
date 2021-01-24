
#include <iostream>
#include <list>

void f(const std::list<std::string *> & l) {
	std::string *	found = *l.begin();
	found->append("123");
}

int main() {
	std::list<std::string *>	l;
	l.push_back(new std::string("abc"));

	f(l);
	std::cout << *l.front() << std::endl;
}
