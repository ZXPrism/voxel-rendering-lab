#include <utils.h>

#include <fstream>
#include <sstream>

namespace vox::utils {

std::string load_whole_file(const std::string &path) {
	std::ifstream fin(path);
	std::stringstream ssm;
	ssm << fin.rdbuf();
	std::string result = ssm.str();
	fin.close();
	return result;
}

}  // namespace vox::utils
