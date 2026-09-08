# Compatibility entry points; CMake and its injected Conan provider own the build.
CMAKE ?= cmake
CONFIG ?= release
JOBS ?= 4
PREFIX ?= /usr/local
CMAKE_ARGS ?=
SHIJIMA_DEPLOY_QT ?= OFF
SHIJIMA_USE_QTMULTIMEDIA ?= ON

ifeq ($(CONFIG),debug)
CMAKE_CONFIG := Debug
else ifeq ($(CONFIG),release)
CMAKE_CONFIG := Release
else
$(error Invalid CONFIG. Use CONFIG=debug or CONFIG=release)
endif

ifeq ($(OS),Windows_NT)
PLATFORM := Windows
else ifneq ($(findstring mingw,$(CC)),)
PLATFORM := Windows
else ifeq ($(shell uname -s),Darwin)
PLATFORM := macOS
else
PLATFORM := Linux
endif

BUILD_DIR ?= $(CURDIR)/build/$(PLATFORM)/$(CONFIG)
PUBLISH_DIR ?= $(CURDIR)/publish/$(PLATFORM)/$(CONFIG)

.PHONY: all configure build clean install appimage macapp
all: build
	$(CMAKE) --install "$(BUILD_DIR)" --prefix "$(PUBLISH_DIR)"

configure:
	$(CMAKE) -S . -B "$(BUILD_DIR)" -G Ninja -DCMAKE_BUILD_TYPE=$(CMAKE_CONFIG) \
		-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES="$(CURDIR)/conan_provider.cmake" \
		-DSHIJIMA_USE_QTMULTIMEDIA=$(SHIJIMA_USE_QTMULTIMEDIA) \
		-DSHIJIMA_DEPLOY_QT=$(SHIJIMA_DEPLOY_QT) $(CMAKE_ARGS)

build: configure
	$(CMAKE) --build "$(BUILD_DIR)" --parallel $(JOBS)

clean:
	if [ -f "$(BUILD_DIR)/CMakeCache.txt" ]; then $(CMAKE) --build "$(BUILD_DIR)" --target clean; fi

install: build
	$(CMAKE) --install "$(BUILD_DIR)" --prefix "$(DESTDIR)$(PREFIX)"

macapp: SHIJIMA_DEPLOY_QT=ON
macapp: all

# Keep AppImage packaging available on Linux after the CMake install step.
APPIMAGE_ARCH := $(shell uname -m)
APPIMAGE_TOOLS := linuxdeploy linuxdeploy-plugin-qt linuxdeploy-plugin-appimage
APPIMAGE_FILES := $(addprefix $(BUILD_DIR)/,$(addsuffix -$(APPIMAGE_ARCH).AppImage,$(APPIMAGE_TOOLS)))

$(APPIMAGE_FILES):
	mkdir -p "$(BUILD_DIR)"
	curl --fail --location --output "$@" \
		"https://github.com/linuxdeploy/$(patsubst %-$(APPIMAGE_ARCH).AppImage,%,$(notdir $@))/releases/latest/download/$(notdir $@)"
	chmod +x "$@"

appimage: all $(APPIMAGE_FILES)
	cd "$(BUILD_DIR)" && NO_STRIP=1 APPIMAGE_EXTRACT_AND_RUN=1 \
		./linuxdeploy-$(APPIMAGE_ARCH).AppImage --appdir AppDir \
		--executable "$(PUBLISH_DIR)/bin/shijima-qt" \
		--desktop-file "$(CURDIR)/com.pixelomer.ShijimaQt.desktop" \
		--icon-file "$(CURDIR)/com.pixelomer.ShijimaQt.png" --plugin qt --output appimage
	cp "$(BUILD_DIR)"/Shijima-Qt-*.AppImage "$(PUBLISH_DIR)/Shijima-Qt.AppImage"
