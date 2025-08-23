#pragma once

namespace vrl {

template<typename Derived>
class Singleton {
public:
	Singleton(const Singleton &) = delete;
	Singleton &operator=(const Singleton &) = delete;

	static Derived &instance() {
		static Derived inst;
		return inst;
	}

protected:
	Singleton() = default;
	~Singleton() = default;
};

}  // namespace vrl
