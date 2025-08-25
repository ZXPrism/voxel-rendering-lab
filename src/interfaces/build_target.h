#pragma once

namespace vrl {

template<typename Derived>
class IBuildTarget {
private:
	bool _IsComplete = false;

public:
	bool is_complete() const {
		return _IsComplete;
	}

	void _set_complete() {
		_IsComplete = true;
	}

protected:
	IBuildTarget() = default;
	~IBuildTarget() = default;
};

}  // namespace vrl
