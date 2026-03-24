# Perbaikan Makefile untuk GitHub Actions
ANDROID_SDK ?= /usr/local/lib/android/sdk
NDK_PATH ?= $(ANDROID_SDK)/ndk/26.1.10909125
BUILD_TOOLS_VERSION := $(shell ls $(ANDROID_SDK)/build-tools | tail -n 1)
PLATFORM_VERSION := $(shell ls $(ANDROID_SDK)/platforms | tail -n 1)

build:
	@echo "Building native code..."
	$(NDK_PATH)/ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=jni/Android.mk NDK_APPLICATION_MK=jni/Application.mk
	@echo "Compiling Dummy.java..."
	javac Dummy.java
	@echo "Converting to classes.dex..."
	d8 --output . Dummy.class
	@echo "Packaging APK..."
	aapt package -f -M AndroidManifest.xml -S res -I $(ANDROID_SDK)/platforms/$(PLATFORM_VERSION)/android.jar -F HelloWorld.unaligned.apk
	@echo "Adding libs and dex..."
	mkdir -p lib
	cp -r libs/* lib/
	zip -u HelloWorld.unaligned.apk lib/*/libhello.so classes.dex
	@echo "Aligning and Signing..."
	zipalign -f -v 4 HelloWorld.unaligned.apk HelloWorld.apk
	apksigner sign --ks debug.keystore --ks-key-alias androiddebugkey --ks-pass pass:android --key-pass pass:android --out HelloWorld-signed.apk HelloWorld.apk

clean:
	rm -rf libs obj lib classes.dex Dummy.class HelloWorld.unaligned.apk HelloWorld.apk HelloWorld-signed.apk
