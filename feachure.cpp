#include "Configuration.hpp"

int main() {
	int ac = 3;
	const char * av[] = {"./ircserv", "6668", "pass"};

	try {
		Configuration	conf(ac, av);
	}
	catch (std::runtime_error & error) {
		std::cout << error.what() << std::endl;
	}
}
