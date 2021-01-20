
#include <iostream>

int main() {
	std::string		tmp("abc\x0D\x0A def\x0D\x0A");
	static const char *				crlf = "\x0D\x0A";

	std::cout << tmp.substr(0, tmp.find(crlf) + 2) << "$" << std::endl;
	std::cout << tmp.substr(tmp.find(crlf) + 2, tmp.size() - tmp.find(crlf) - 2) << "$" << std::endl;

	tmp.erase(0, tmp.find(crlf) + 2);
	std::cout << tmp << '$' << std::endl;
}
