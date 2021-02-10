#include "Configuration.hpp"

int main() {
	int ac = 3;
	const char * av[] = {"./ircserv", "6668", "pass"};

	try {
		Configuration	conf(ac, av);
		std::cout << "Success!" << std::endl;
	}
	catch (std::runtime_error & error) {
		std::cout << error.what() << std::endl;
	}
}
