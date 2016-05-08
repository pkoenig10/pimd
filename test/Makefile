TESTS = bandwidth flop saxpy

BIN = bin
TARGET = $(addprefix $(BIN)/,$(TESTS))

CXX = g++
CXXFLAGS += -std=c++11 -O0 -Wall -Wextra -mfpu=neon -fno-tree-vectorize -lpimd
INSTALL = install
RM = rm -f

.PHONY: all clean

all: $(BIN) $(TARGET)

$(BIN)/%:  %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(BIN):
	$(INSTALL) -d $(BIN)

clean:
	$(RM) -r $(BIN)
