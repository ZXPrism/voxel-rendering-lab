#pragma once

#include <memory>
#include <string>
#include <utility>

namespace vox {

template<typename Derived, typename BuildTarget>
class IBuilder {
public:
	explicit IBuilder(std::string name)
	    : _name(std::move(name)) {
	}

	[[nodiscard]] std::string get_name() const { return _name; }

	[[nodiscard]] std::shared_ptr<BuildTarget> build() const {
		return static_cast<const Derived *>(this)->_build();
	}

protected:
	std::string _name;
};

}  // namespace vox
