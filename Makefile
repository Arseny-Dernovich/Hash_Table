CXX := g++
SRC := Hash_Table.cpp
OBJ := Hash_Table.o
OUT := hash

CXXFLAGS := -std=c++17 -O1	 -g -pg -ggdb3 -fno-omit-frame-pointer -fno-inline \
-Wall -Wextra -Wno-unused-parameter

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUT) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OUT) $(OBJ)
