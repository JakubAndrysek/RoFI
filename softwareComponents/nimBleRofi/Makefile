# Define variables
VENV_DIR = .venv
PYTHON = python3.12
REQUIREMENTS = requirements.txt
PROTO_SRC_DIR = ../nimBleProtocol
PROTO_FILE = $(PROTO_SRC_DIR)/rofi.proto
NANO_OUT_DIR = src/protoc

# Create a virtual environment
$(VENV_DIR)/bin/activate:
	$(PYTHON) -m venv $(VENV_DIR)
	. $(VENV_DIR)/bin/activate

# Install requirements
install: $(VENV_DIR)/bin/activate
	. $(VENV_DIR)/bin/activate && pip install -r $(REQUIREMENTS)

# Generate protocol buffers with nanopb
generate-protocol: install
	mkdir -p $(NANO_OUT_DIR)
	. $(VENV_DIR)/bin/activate && $(PYTHON) -m grpc_tools.protoc --proto_path=$(PROTO_SRC_DIR) --nanopb_out=$(NANO_OUT_DIR) $(PROTO_FILE)

# Clean the virtual environment
clean:
	rm -rf $(VENV_DIR) $(NANO_OUT_DIR)

# Default target
all: install generate-protocol

.PHONY: install clean all generate-protocol
