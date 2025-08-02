test_pool.out: ./tests/pool.cpp ./srcs/data_structure/pool.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
