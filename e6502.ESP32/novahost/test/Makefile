# Host-side unit tests for ndi_image. Builds with clang++ on macOS.
# Run: `make test`

CXX      ?= clang++
CXXFLAGS  = -std=c++17 -Wall -Wextra -Wno-unused-parameter -O0 -g -I..
TARGETS   = test_ndi

.PHONY: all test clean

all: $(TARGETS)

test_ndi: test_ndi.cpp ../ndi_image.cpp ../ndi_image.h
	$(CXX) $(CXXFLAGS) -o $@ test_ndi.cpp ../ndi_image.cpp

test: $(TARGETS)
	./test_ndi
	python3 test_wifi_contract.py

clean:
	rm -f $(TARGETS)
