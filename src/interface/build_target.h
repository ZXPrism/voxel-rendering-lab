#pragma once

#include <string>

namespace vox {

template<typename Derived>
class IBuildTarget {
protected:
	std::string _name;

private:
	bool _is_complete = false;

public:
	[[nodiscard]] bool is_complete() const {
		return _is_complete;
	}

	void _set_complete() {
		_is_complete = true;
	}

	[[nodiscard]] std::string get_name() const {
		return _name;
	}

	void _set_name(const std::string &name) {
		_name = name;
	}
};

}  // namespace vox
