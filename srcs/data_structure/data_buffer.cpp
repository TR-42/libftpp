#include "./data_buffer.hpp"

DataBuffer::DataBuffer() {}

DataBuffer::~DataBuffer()
{
	for (auto [ptr, _] : this->_buf)
		delete[] ptr;
	this->_buf.clear();
}

DataBuffer::DataBuffer(const DataBuffer &src)
{
	for (const auto [ptr, size] : src._buf) {
		uint8_t *new_buf = new uint8_t[size];
		std::memcpy(new_buf, ptr, size);
		this->_buf.push_back(std::make_tuple(new_buf, size));
	}
}

DataBuffer &DataBuffer::operator=(const DataBuffer &src)
{
	if (this == &src) {
		return *this;
	}

	for (auto [ptr, _] : this->_buf)
		delete[] ptr;
	this->_buf.clear();

	for (const auto [ptr, size] : src._buf) {
		uint8_t *new_buf = new uint8_t[size];
		std::memcpy(new_buf, ptr, size);
		this->_buf.push_back(std::make_tuple(new_buf, size));
	}
	return *this;
}
