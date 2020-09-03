#include "pwTextParser.hpp"
#include <fstream>
#include <streambuf>
#include <iostream>

#ifdef _UNICODE
int main(int, char**) {
	std::wstring		sText;
	std::wifstream 		f(L"config.txt");

	if (f.is_open()) {
		f.seekg(0, std::ifstream::end);
		sText.reserve(static_cast<std::size_t>(f.tellg()));
		f.seekg(0, std::ifstream::beg);
		sText.assign((std::istreambuf_iterator<wchar_t>(f)), std::istreambuf_iterator<wchar_t>());
		f.close();
	}

	if (!sText.empty()) {
		pw::TextParser p(sText);
		if (p) {
			for (auto& pair : p) {
				std::wcout << pair.first << " => " << pair.second << std::endl;
			}
		}
	}
}
#else
int main(int, char**) {
	std::string		sText;
	std::ifstream 	f("config.txt");

	if (f.is_open()) {
		f.seekg(0, std::ifstream::end);
		sText.reserve(static_cast<std::size_t>(f.tellg()));
		f.seekg(0, std::ifstream::beg);
		sText.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
		f.close();
	}

	if (!sText.empty()) {
		pw::TextParser p(sText);
		if (p) {
			for (auto& pair : p) {
				std::cout << pair.first << " => " << pair.second << std::endl;
			}
		}
	}
}
#endif