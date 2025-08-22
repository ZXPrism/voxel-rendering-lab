#pragma once

#include <string>

namespace vrl {

template<typename Derived>
class IBuilder {
protected:
	std::string _Name;

public:
	IBuilder(const std::string &name)
	    : _Name(name) {
	}

	void build() const {
		static_cast<Derived *>(this)->_build();
	}
};

}  // namespace vrl
