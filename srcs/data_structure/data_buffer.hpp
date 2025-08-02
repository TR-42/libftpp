#pragma once

#include <cstring>
#include <tuple>
#include <vector>

class DataBuffer
{
 private:
	std::vector<std::tuple<uint8_t *, size_t> > _buf;

 public:
	DataBuffer();
	virtual ~DataBuffer();
	DataBuffer(const DataBuffer &src);
	DataBuffer &operator=(const DataBuffer &src);

	template <typename T>
	DataBuffer &operator<<(const T &data)
	{
		const uint8_t *ptr = reinterpret_cast<uint8_t *>(const_cast<T *>(&data));
		const size_t size = sizeof(T);

		uint8_t *new_buf = new uint8_t[size];
		std::memcpy(new_buf, ptr, size);
		this->_buf.push_back(std::make_tuple(new_buf, size));

		return *this;
	}

	template <typename T>
	DataBuffer &operator>>(T &data)
	{
		if (this->_buf.empty())
			throw std::runtime_error("DataBuffer is empty");

		auto [ptr, size] = this->_buf.back();

		if (size < sizeof(T))
			throw std::runtime_error("Not enough data in DataBuffer");

		this->_buf.pop_back();

		std::memcpy(&data, ptr, size);
		delete[] ptr;

		return *this;
	}
};
