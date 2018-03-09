#ifndef __PLUTO_SINGLETON_H__
#define __PLUTO_SINGLETON_H__

#include <cassert>

namespace pluto
{

template<typename T>
class Singleton
{
protected:
	static T* singleton_;

public:
	Singleton(void)
	{
		assert(!singleton_);

		singleton_ = static_cast<T*>(this);
	}

	~Singleton(void)
	{
		assert(singleton_);

		singleton_ = 0;
	}

	static T& getSingleton(void)
	{
		assert(singleton_);

		return (*singleton_);
	}

	static T* getSingletonPtr(void)
	{
		return singleton_;
	}

};

#define PLUTO_SINGLETON_INIT(TYPE)					\
template<> TYPE* Singleton<TYPE>::singleton_ = 0;	\

}

#endif
