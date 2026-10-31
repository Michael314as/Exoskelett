# Simulation und Abnahmetests auf dem PC: make test
CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -Werror

test: build/sim_test
	./build/sim_test

build/sim_test: test/sim_test.cpp src/exo_core.h src/exo_app.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Isrc test/sim_test.cpp -o $@

clean:
	rm -rf build

.PHONY: test clean
