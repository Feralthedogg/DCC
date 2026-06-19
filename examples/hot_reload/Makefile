ROOT := $(abspath ../..)
SOURCE_TREE := $(if $(wildcard $(ROOT)/CMakeLists.txt),1,0)
CMAKE_BUILD ?= $(ROOT)/build
LOCAL_BUILD := build
CMAKE_CACHE := $(CMAKE_BUILD)/CMakeCache.txt
CONFIGURE_STAMP := $(LOCAL_BUILD)/cmake-configured.stamp
HOST_NAME := dcc_hot_reload_test_host
MODULE_NAME := dcc_hot_reload_test_bot
WORKER_NAME := dcc_hot_reload_worker
HOST := $(LOCAL_BUILD)/$(HOST_NAME)
MODULE := $(LOCAL_BUILD)/$(MODULE_NAME).so
WORKER := $(LOCAL_BUILD)/$(WORKER_NAME)
STAMP_HEADER := $(LOCAL_BUILD)/hot_reload_stamp.h
BUILD_STAMP := $(shell perl -MTime::HiRes=time -e 'printf "%.6f", time')
INSTALL_PREFIX ?= $(abspath ../../../..)
CMAKE_PREFIX_PATH ?= $(INSTALL_PREFIX)
HOST_BIN ?= $(INSTALL_PREFIX)/bin/dcc_hot_reload_host
WORKER_BIN ?= $(INSTALL_PREFIX)/bin/dcc_hot_reload_worker
DCC_HOT_RELOAD_HEALTH_ADDRESS ?= 127.0.0.1
DCC_HOT_RELOAD_HEALTH_PORT ?= 18080
INSTALLED_CMAKE_SOURCE := $(LOCAL_BUILD)/installed-module-src
INSTALLED_CMAKE_BUILD := $(LOCAL_BUILD)/installed-module-build
INSTALLED_CMAKE_FILE := $(INSTALLED_CMAKE_SOURCE)/CMakeLists.txt
CMAKE_BUILD_TYPE ?= Debug
DCC_INSTALLED_CMAKE_ARGS :=
ifneq ($(strip $(DCC_LLAM_ROOT)),)
DCC_INSTALLED_CMAKE_ARGS += -DDCC_LLAM_ROOT="$(DCC_LLAM_ROOT)"
endif
ifneq ($(strip $(DCC_LLAM_LIBRARY)),)
DCC_INSTALLED_CMAKE_ARGS += -DDCC_LLAM_LIBRARY="$(DCC_LLAM_LIBRARY)"
endif

.PHONY: all stamp cmake-build installed-cmake-source installed-cmake-build host module run live-soak clean

all: host module

stamp:
	mkdir -p $(LOCAL_BUILD)
	printf '%s\n' \
		'#ifndef DCC_HOT_RELOAD_STAMP_H' \
		'#define DCC_HOT_RELOAD_STAMP_H' \
		'#define DCC_HOT_RELOAD_TEST_BUILD_STAMP "$(BUILD_STAMP)"' \
		'#endif' > $(STAMP_HEADER)

ifeq ($(SOURCE_TREE),1)

cmake-build: stamp
	@if [ ! -f "$(CMAKE_CACHE)" ] || [ ! -f "$(CONFIGURE_STAMP)" ] || [ "$(ROOT)/CMakeLists.txt" -nt "$(CONFIGURE_STAMP)" ]; then \
		cmake -S "$(ROOT)" -B "$(CMAKE_BUILD)"; \
		touch "$(CONFIGURE_STAMP)"; \
	fi
	cmake --build "$(CMAKE_BUILD)" --target $(HOST_NAME) $(MODULE_NAME) $(WORKER_NAME)

host: cmake-build
	mkdir -p $(LOCAL_BUILD)
	cp "$(CMAKE_BUILD)/$(HOST_NAME)" "$(HOST)"
	cp "$(CMAKE_BUILD)/$(WORKER_NAME)" "$(WORKER)"

module: cmake-build
	mkdir -p $(LOCAL_BUILD)
	cp "$(CMAKE_BUILD)/$(MODULE_NAME).so" "$(MODULE).tmp"
	mv "$(MODULE).tmp" "$(MODULE)"
	@echo "hot reload module updated: $(MODULE)"

else

installed-cmake-source:
	mkdir -p "$(INSTALLED_CMAKE_SOURCE)"
	printf '%s\n' \
		'cmake_minimum_required(VERSION 3.20)' \
		'project(dcc_installed_hot_reload_example LANGUAGES C)' \
		'set(CMAKE_C_STANDARD 11)' \
		'set(CMAKE_C_STANDARD_REQUIRED ON)' \
		'set(CMAKE_C_EXTENSIONS OFF)' \
		'find_package(dcc CONFIG REQUIRED)' \
		'dcc_add_hot_reload_module($(MODULE_NAME)' \
		'    SOURCES' \
		'        "$(abspath bot.c)"' \
		')' \
		'target_include_directories($(MODULE_NAME) PRIVATE "$${CMAKE_CURRENT_LIST_DIR}/..")' \
		> "$(INSTALLED_CMAKE_FILE).tmp"
	mv "$(INSTALLED_CMAKE_FILE).tmp" "$(INSTALLED_CMAKE_FILE)"

installed-cmake-build: stamp installed-cmake-source
	cmake -S "$(INSTALLED_CMAKE_SOURCE)" -B "$(INSTALLED_CMAKE_BUILD)" \
		-DCMAKE_BUILD_TYPE="$(CMAKE_BUILD_TYPE)" \
		-DCMAKE_PREFIX_PATH="$(CMAKE_PREFIX_PATH)" \
		$(DCC_INSTALLED_CMAKE_ARGS)
	cmake --build "$(INSTALLED_CMAKE_BUILD)" --target $(MODULE_NAME)

host:
	mkdir -p $(LOCAL_BUILD)
	@if [ ! -x "$(HOST_BIN)" ]; then \
		printf 'installed dcc_hot_reload_host not found: %s\n' "$(HOST_BIN)" >&2; \
		exit 1; \
	fi
	@if [ ! -x "$(WORKER_BIN)" ]; then \
		printf 'installed dcc_hot_reload_worker not found: %s\n' "$(WORKER_BIN)" >&2; \
		exit 1; \
	fi
	cp "$(HOST_BIN)" "$(HOST)"
	cp "$(WORKER_BIN)" "$(WORKER)"

module: installed-cmake-build
	mkdir -p $(LOCAL_BUILD)
	@artifact=""; \
	for candidate in \
		"$(INSTALLED_CMAKE_BUILD)/$(MODULE_NAME).so" \
		"$(INSTALLED_CMAKE_BUILD)/Debug/$(MODULE_NAME).so" \
		"$(INSTALLED_CMAKE_BUILD)/Release/$(MODULE_NAME).so" \
		"$(INSTALLED_CMAKE_BUILD)/RelWithDebInfo/$(MODULE_NAME).so"; do \
		if [ -f "$$candidate" ]; then \
			artifact=$$candidate; \
			break; \
		fi; \
	done; \
	if [ -z "$$artifact" ]; then \
		printf 'built hot reload module was not found under %s\n' "$(INSTALLED_CMAKE_BUILD)" >&2; \
		exit 1; \
	fi; \
	cp "$$artifact" "$(MODULE).tmp"
	mv "$(MODULE).tmp" "$(MODULE)"
	@echo "hot reload module updated: $(MODULE)"

endif

run: all
	DCC_HOT_RELOAD_HEALTH_ADDRESS="$(DCC_HOT_RELOAD_HEALTH_ADDRESS)" \
	DCC_HOT_RELOAD_HEALTH_PORT="$(DCC_HOT_RELOAD_HEALTH_PORT)" \
	"./$(HOST)" $(HOST_ARGS) "$(MODULE)" "$(WORKER)"

live-soak: all
	sh ./live_soak.sh

clean:
	rm -rf $(LOCAL_BUILD)
