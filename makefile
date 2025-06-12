CXX = g++

CXXFLAGS = -pg -O2 -g

TARGET = Engine

TARGET_DEL = Engine.exe

SRCS = main.cpp core/board.cpp core/eval.cpp core/movegen.cpp core/search.cpp core/transpoTable.cpp core/zobrist.cpp util/debug.cpp util/timing.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET) run

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	del $(TARGET_DEL) main.o core\board.o core\eval.o core\movegen.o core\search.o core\transpoTable.o core\zobrist.o util\debug.o util\timing.o

