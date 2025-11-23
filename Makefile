CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

SRCS = memory.cpp read.cpp print.cpp eval.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = autolisp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

test: $(TARGET)
	./$(TARGET) --test

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean test
