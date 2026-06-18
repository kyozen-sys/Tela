.PHONY: all build configure examples clean rebuild

COMPILE_DIR := ./build

CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=Debug

all: build

configure:
	cmake -S . -B $(COMPILE_DIR) $(CMAKE_FLAGS)

build: configure
	cmake --build $(COMPILE_DIR) --parallel

examples:
	cmake -S . -B $(COMPILE_DIR) -DBUILD_EXAMPLES=ON $(CMAKE_FLAGS)
	cmake --build $(COMPILE_DIR) --parallel

clean:
	cmake --build $(COMPILE_DIR) --target clean

rebuild: clean build