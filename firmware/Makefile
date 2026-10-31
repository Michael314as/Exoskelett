# Simulation und Abnahmetests auf dem PC: make test
CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -Werror

test: build/sim_test build/teensy_host build/esp32_host
	./build/sim_test
	./build/teensy_host && echo "Teensy-Code: kompiliert und läuft (mit Platzhaltern)"
	./build/esp32_host

build/sim_test: test/sim_test.cpp src/exo_core.h src/exo_app.h src/drivers.h src/settings.h esp32/status.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Isrc test/sim_test.cpp -o $@

build/teensy_host: test/teensy_host.cpp src/main_teensy.cpp src/exo_core.h src/exo_app.h src/drivers.h src/settings.h test/stubs/EEPROM.h test/stubs/Arduino.h test/stubs/FlexCAN_T4.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -DARDUINO -DEXO_HOST_TEST -Itest/stubs -Isrc test/teensy_host.cpp -o $@

build/esp32_host: test/esp32_host.cpp esp32/esp32_display_ble.ino esp32/status.h test/stubs_esp32/Arduino.h test/stubs_esp32/GxEPD2_BW.h test/stubs_esp32/NimBLEDevice.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Itest/stubs_esp32 -x c++ test/esp32_host.cpp -o $@

# Auswertung prüfen: Simulations-Log mit bekannten Werten → tools/auswertung.py muss sie wiederfinden
ident: build/log_demo
	./build/log_demo > build/log_demo.csv
	python3 ../tools/auswertung.py build/log_demo.csv --erwartet 0.05,0.2,0.3,0.9

build/log_demo: test/log_demo.cpp src/exo_core.h src/exo_app.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Isrc test/log_demo.cpp -o $@

clean:
	rm -rf build

.PHONY: test ident clean
