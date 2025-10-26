.PHONY: config res game run test

config:
	cmake -S . -B ./build -DCMAKE_SYSTEM_NAME=Linux -G "Ninja Multi-Config"

res:
ifneq (,$(wildcard ./build/resources))
	rm ./build/resources
endif
ifneq (,$(wildcard ./assets/resources))
	rm ./assets/resources
endif
	cmake --build build --config Release --target resource_packer
	./build/tools/resource_packer/Release/resource_packer ./assets ./assets/resources

game:
	cmake --build build --config Debug --target nathan_stream

run: game
# 	DRI_PRIME=1 ./build/src/Debug/nathan_stream ./assets
	DRI_PRIME=1 ./build/src/Debug/nathan_stream ./assets -m 2

clean:
	cmake --build build --target clean

test:
	cmake --build build --config Debug --target unit_tests
	ctest --test-dir ./build -C Debug --progress -j

