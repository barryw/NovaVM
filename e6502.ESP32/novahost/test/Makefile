# Host-side unit tests for ndi_image. Builds with clang++ on macOS.
# Run: `make test`

CXX      ?= clang++
CXXFLAGS  = -std=c++17 -Wall -Wextra -Wno-unused-parameter -O0 -g -I..
TARGETS   = test_ndi test_midi_engine test_sid_vm test_boot_config_parser test_shelf_alloc

.PHONY: all test clean

all: $(TARGETS)

test_ndi: test_ndi.cpp ../ndi_image.cpp ../ndi_image.h
	$(CXX) $(CXXFLAGS) -o $@ test_ndi.cpp ../ndi_image.cpp

test_midi_engine: test_midi_engine.cpp ../midi_engine.cpp ../midi_engine.h
	$(CXX) $(CXXFLAGS) -o $@ test_midi_engine.cpp ../midi_engine.cpp

test_sid_vm: test_sid_vm.cpp ../sid_vm.cpp ../sid_vm.h
	$(CXX) $(CXXFLAGS) -o $@ test_sid_vm.cpp ../sid_vm.cpp

test_boot_config_parser: test_boot_config_parser.cpp ../boot_config_parser.cpp ../boot_config_parser.h
	$(CXX) $(CXXFLAGS) -o $@ test_boot_config_parser.cpp ../boot_config_parser.cpp

test_shelf_alloc: test_shelf_alloc.cpp ../shelf_alloc.h
	$(CXX) $(CXXFLAGS) -o $@ test_shelf_alloc.cpp

test: $(TARGETS)
	./test_ndi
	./test_midi_engine
	./test_sid_vm
	./test_boot_config_parser
	./test_shelf_alloc
	python3 test_wifi_contract.py

clean:
	rm -f $(TARGETS)
