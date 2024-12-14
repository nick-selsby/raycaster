# compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++11 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

# source files
SRCS = main.cpp
TARGET = raycaster

# build rules
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# run the application
run: $(TARGET)
	./$(TARGET)

# clean build files
clean:
	rm -f $(TARGET)

.PHONY: all run clean
