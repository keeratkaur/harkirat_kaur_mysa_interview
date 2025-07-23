CXX = g++
CXXFLAGS = -std=c++11 -Iinclude
SRC = $(wildcard src/*.cpp) main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = mysa_irrigation

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.o src/*.o 