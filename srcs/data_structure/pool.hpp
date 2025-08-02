#pragma once

#include <list>
#include <set>

template <typename TType>
class Pool
{
 public:
	class Object
	{
		friend class Pool<TType>;

	 private:
		bool _is_in_use;
		TType *_obj;

		template <typename... TArgs>
		void _obj_ctor(TArgs &&...p_args)
		{
			if (this->_is_in_use) {
				this->_obj_dtor();
			}
			new (this->_obj) TType(p_args...);
			this->_is_in_use = true;
		}
		void _obj_dtor()
		{
			if (!this->_is_in_use) {
				return;
			}
			this->_obj->~TType();
			this->_is_in_use = false;
		}

		void _dtor()
		{
			if (this->_is_in_use) {
				this->_obj_dtor();
			}
			operator delete(this->_obj);
			this->_obj = nullptr;
		}

	 public:
		Object() : _is_in_use(false)
		{
			this->_obj = reinterpret_cast<TType *>(operator new(sizeof(TType)));
		}
		Object(const Object &src) : _is_in_use(src._is_in_use), _obj(src._obj) {}
		Object &operator=(const Object &src)
		{
			if (this == &src) {
				return *this;
			}
			this->_is_in_use = src._is_in_use;
			this->_obj = src._obj;
			return *this;
		}
		virtual ~Object() = default;

		TType *operator->()
		{
			return this->_obj;
		}

		bool operator==(const Object &other) const
		{
			return this->_obj == other._obj;
		}

		bool operator!=(const Object &other) const
		{
			return !(*this == other);
		}

		bool operator<(const Object &other) const
		{
			return this->_obj < other._obj;
		}
		bool operator>(const Object &other) const
		{
			return this->_obj > other._obj;
		}
		bool operator<=(const Object &other) const
		{
			return this->_obj <= other._obj;
		}
		bool operator>=(const Object &other) const
		{
			return this->_obj >= other._obj;
		}
	};

 private:
	std::list<typename Pool<TType>::Object> _pooled_objects;
	std::set<typename Pool<TType>::Object> _acquired_objects;

	Pool(const Pool<TType> &) = delete;
	Pool &operator=(const Pool<TType> &) = delete;

 public:
	Pool() = default;
	virtual ~Pool()
	{
		for (auto &obj : this->_pooled_objects) {
			obj._dtor();
		}
		this->_pooled_objects.clear();
		for (auto &obj : this->_acquired_objects) {
			obj._dtor();
		}
		this->_acquired_objects.clear();
	}

	void resize(const size_t &numberOfObjectStored)
	{
		const size_t currentSize = this->_pooled_objects.size() + this->_acquired_objects.size();
		if (currentSize == numberOfObjectStored) {
			return;
		}

		if (numberOfObjectStored < currentSize) {
			const size_t toRemove = currentSize - numberOfObjectStored;
			if (this->_pooled_objects.size() < toRemove) {
				throw std::runtime_error("Cannot remove more objects than available in the pool.");
			}
			for (size_t i = 0; i < toRemove; ++i) {
				Pool<TType>::Object v = this->_pooled_objects.front();
				this->_pooled_objects.pop_front();
				v._dtor();
			}
		} else {
			this->_pooled_objects.resize(numberOfObjectStored - this->_acquired_objects.size());
		}
	}

	template <typename... TArgs>
	typename Pool<TType>::Object acquire(TArgs &&...p_args)
	{
		if (this->_pooled_objects.empty()) {
			throw std::runtime_error("No available objects in the pool.");
		}
		Pool<TType>::Object v = this->_pooled_objects.front();
		this->_pooled_objects.pop_front();
		v._obj_ctor(p_args...);
		this->_acquired_objects.insert(v);
		return v;
	}

	void release(Pool<TType>::Object &p_obj)
	{
		p_obj._obj_dtor();
		this->_acquired_objects.erase(p_obj);
		this->_pooled_objects.push_back(p_obj);
	}
};
