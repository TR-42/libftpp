#include "../srcs/data_structure/pool.hpp"

#include <iostream>
#include <string>

int main(void)
{
	Pool<std::string> pool;
	pool.resize(10);

	typename Pool<std::string>::Object obj = pool.acquire("Hello, World!");

	std::cout << "Acquired object length: " << obj->length() << std::endl;

	pool.release(obj);

	return 0;
}
