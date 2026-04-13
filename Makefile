ANDROID_SDK ?= /usr/local/lib/android/sdk
NDK_PATH ?= $(ANDROID_SDK)/ndk/26.1.10909125
# Cari build-tools secara otomatis
BUILD_TOOLS_PATH := $(shell ls -d $(ANDROID_SDK)/build-tools/* | tail -1)
PLATFORM_JAR := $(shell ls -d $(ANDROID_SDK)/platforms/android-* | tail -1)/android.jar

# Alias untuk tools agar tidak perlu PATH global
AAPT := $(BUILD_TOOLS_PATH)/aapt
D8 := $(BUILD_TOOLS_PATH)/d8
ZIPALIGN := $(BUILD_TOOLS_PATH)/zipalign
APKSIGNER := $(BUILD_TOOLS_PATH)/apksigner

build:
	@echo "Using Build Tools: $(BUILD_TOOLS_PATH)"
	@echo "Building native code..."
	$(NDK_PATH)/ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=jni/Android.mk NDK_APPLICATION_MK=jni/Application.mk
	
	@echo "Compiling & Converting Java..."
	javac Dummy.java
	$(D8) --output . Dummy.class
	
	@echo "Packaging APK..."
	$(AAPT) package -f -M AndroidManifest.xml -S res -I $(PLATFORM_JAR) -F HelloWorld.unaligned.apk
	
	@echo "Adding libs and dex..."
	mkdir -p lib && 
	zip -u HelloWorld.unaligned.apk lib/*/libhello.so classes.dex
	
	@echo "Aligning..."
	$(ZIPALIGN) -f 4 HelloWorld.unaligned.apk zamera.apk
	
	@echo "Signing..."
	$(APKSIGNER) sign --ks debug.keystore --ks-key-alias androiddebugkey --ks-pass pass:android --key-pass pass:android --out zamera-final.apk zamera.apk
	
	@echo "Cleaning up..."
	rm -rf libs obj lib classes.dex Dummy.class HelloWorld.unaligned.apk zamera.apk debug.keystore

clean:
	rm -rf libs obj lib classes.dex Dummy.class *.apk
