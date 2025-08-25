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

	BuildTarget build() {
		return static_cast<Derived *>(this)->_build();
	}

protected:
	IBuilder() = default;
	~IBuilder() = default;
};

}  // namespace vrl
