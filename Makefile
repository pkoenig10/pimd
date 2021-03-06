OBJECT_DIR = obj
QASM_DIR = qasm
HEX_DIR = hex
HELPERS_DIR = .
MAILBOX_DIR = /opt/vc/src/hello_pi/hello_fft
INSTALL_PATH = /usr/local

TARGET = $(OBJECT_DIR)/libpimd.so
SOURCES = pimd.cpp pimd_op.cpp
HEADERS = pimd.h 
OBJECTS = $(addprefix $(OBJECT_DIR)/,$(SOURCES:.cpp=.o))
QASM = $(wildcard $(QASM_DIR)/*.qasm)
QINC = $(wildcard $(QASM_DIR)/*.qinc)
HEX = $(patsubst $(QASM_DIR)/%.qasm,$(HEX_DIR)/%.hex,$(QASM))

AS = vc4asm
ASFLAGS = -V
CXX = g++
CXXFLAGS += -std=c++11 -O3 -Wall -Wextra -I $(MAILBOX_DIR)
INSTALL = install
MAKE = make
RM = rm -f

.PHONY: all install uninstall test clean

.PRECIOUS: $(HEX_DIR)/%.hex

all: $(HEX_DIR) $(OBJECT_DIR) $(TARGET)

install: $(TARGET) $(HEADERS)
	$(INSTALL) -d -m 755 $(INSTALL_PATH)/lib
	$(INSTALL) -m 755 $(TARGET) $(INSTALL_PATH)/lib
	$(INSTALL) -d -m 755 $(INSTALL_PATH)/include
	$(INSTALL) -m 755 $(HEADERS) $(INSTALL_PATH)/include

uninstall:
	$(RM) $(INSTALL_PATH)/lib/$(notdir $(TARGET))
	$(RM) $(addprefix $(INSTALL_PATH)/include/,$(HEADERS))

$(TARGET): $(OBJECTS) $(OBJECT_DIR)/mailbox.o
	$(CXX) $(CXXFLAGS) --shared $^ -o $@

$(OBJECT_DIR)/%.o: %.cpp $(HEX)
	$(CXX) $(CXXFLAGS) -c -fPIC $< -o $@

$(OBJECT_DIR)/mailbox.o: $(MAILBOX_DIR)/mailbox.c
	$(CXX) $(CXXFLAGS) -c -fPIC $< -o $@

$(HEX_DIR)/%.hex: $(QASM_DIR)/%.qasm $(QINC)
	$(AS) $(ASFLAGS) -C $@ $<

$(HEX_DIR):
	$(INSTALL) -d $(HEX_DIR)/

$(OBJECT_DIR):
	$(INSTALL) -d $(OBJECT_DIR)/

test:
	$(MAKE) -C test

clean:
	$(RM) -r $(HEX_DIR)/ $(OBJECT_DIR)/
	$(MAKE) -C test clean
