# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pthread

# Target executable
TARGET = ex4.out

# Source files
SRCS = main.cpp ConfigParser.cpp Dispatcher.cpp

# Header files
HEADERS = Config.h ConfigParser.h Producer.h BoundedQueue.h Dispatcher.h CoEditor.h ScreenManager.h

# Object files (derived from source files)
OBJS = $(SRCS:.cpp=.o)

# Default rule to build the target
all: $(TARGET)

# Rule to create the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
