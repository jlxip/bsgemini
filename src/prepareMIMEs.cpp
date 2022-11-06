#include <common.hpp>
#include <iostream>
#include <fstream>

const char* mimetypes = "/etc/mime.types";
std::unordered_map<std::string, std::string> MIMEs;

void prepareMIMEs() {
	std::ifstream file(mimetypes);
	if(!file.is_open()) {
		std::cerr << "Could not open file: " << mimetypes << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string line;
	while(getline(file, line)) {
		if(!line.size())
			continue;
		if(line[0] == '#')
			continue;

		auto first = std::min(line.find(' '), line.find('\t'));
		if(first == std::string::npos)
			continue; // Not cool

		auto second = first;
		while(second < line.size()) {
			if(line[second] == ' ' || line[second] == '\t') {
				++second;
				continue;
			} else break;
		}
		if(second == line.size())
			continue; // Not cool

		std::string mime = line.substr(0, first);
		auto exts = line.substr(second);
		for(auto const& x : split(exts, ' '))
			MIMEs[x] = mime;
	}
}
