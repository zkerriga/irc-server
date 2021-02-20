#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

static bool f(int i) {
	return true;
}

int main() {
	std::vector<int>	ints(5, 7);
	std::list<int>		list;

	std::copy_if(ints.begin(), ints.end(), std::inserter(list, list.begin()), f);
	std::cout << list.size() << std::endl;
	std::cout << list.front() << std::endl;
}
