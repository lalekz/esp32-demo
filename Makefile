CHIP := esp32
ESP_ROOT := $(HOME)/arduino-esp32
BUILD_DIR := $(PWD)/build
UPLOAD_PORT := /dev/ttyUSB0
UPLOAD_SPEED := 115200

include makeEspArduino.mk
