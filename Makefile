CHIP := esp32
ESP_ROOT := $(HOME)/arduino-esp32
BUILD_DIR := $(PWD)/build
UPLOAD_PORT := /dev/ttyUSB0
UPLOAD_SPEED := 115200

EXCLUDE_DIRS := $(addprefix ./libs/ArduinoJson/,fuzzing test third-party)

include makeEspArduino.mk
