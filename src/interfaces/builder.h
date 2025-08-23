#pragma once

#include <string>

namespace vrl {

template<typename Derived, typename BuildTarget>
class IBuilder {
protected:
	std::string _Name;

public:
	IBuilder(const std::string &name)
	    : _Name(name) {
	}

	std::string get_name() const { return _Name; }

	BuildTarget build() const {
		return static_cast<const Derived *>(this)->_build();
	}
};

}  // namespace vrl
