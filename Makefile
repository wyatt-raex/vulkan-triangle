CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanTriangle: main.cpp
	g++ $(CFLAGS) -o VulkanTriangle.out main.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanTriangle
	./VulkanTriangle.out

clean: 
	rm -f VulkanTriangle.out
