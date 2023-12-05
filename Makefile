CXX = g++
CFLAGS = -std=c++23 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanTest: main.cpp
	$(CXX) $(CFLAGS) -o VulkanTest main.cpp VulkanRenderer.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest
