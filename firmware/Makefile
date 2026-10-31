# Simulation und Abnahmetests auf dem PC: make test
CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -Werror

test: build/sim_test build/teensy_host
	./build/sim_test
	./build/teensy_host && echo "Teensy-Code: kompiliert und läuft (mit Platzhaltern)"

build/sim_test: test/sim_test.cpp src/exo_core.h src/exo_app.h src/drivers.h src/settings.h esp32/status.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Isrc test/sim_test.cpp -o $@

build/teensy_host: test/teensy_host.cpp src/main_teensy.cpp src/exo_core.h src/exo_app.h src/drivers.h src/settings.h test/stubs/EEPROM.h test/stubs/Arduino.h test/stubs/FlexCAN_T4.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -DARDUINO -DEXO_HOST_TEST -Itest/stubs -Isrc test/teensy_host.cpp -o $@

clean:
	rm -rf build

.PHONY: test clean
