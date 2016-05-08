all: any_function-test

any_function-test: any_function-test.cpp ../any_function.h
	$(CXX) any_function-test.cpp -std=c++11 -o $@

clean:
	rm -f any_function-test