/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "graphics_composer_hwc_test"

#include <android-base/logging.h>
#include <composer-vts/2.1/ComposerVts.h>
#include <composer-vts/2.1/GraphicsComposerCallback.h>
#include <composer-vts/2.1/TestCommandReader.h>
#include <gtest/gtest.h>
#include <hardware/hwcomposer2.h>
#include <hidl/GtestPrinter.h>
#include <hidl/ServiceManagement.h>
#include <mapper-vts/2.0/MapperVts.h>
#include <mapper-vts/3.0/MapperVts.h>
#include <mapper-vts/4.0/MapperVts.h>
#include <hidl/HidlTransportSupport.h>
#include <hidl/HidlTransportUtils.h>
#include <android-base/unique_fd.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
namespace android {
namespace hardware {
namespace graphics {
namespace composer {
namespace V2_1 {	
namespace vts {
using android::hardware::graphics::common::V1_0::BufferUsage;
using android::hardware::graphics::common::V1_0::ColorMode;
using android::hardware::graphics::common::V1_0::ColorTransform;
using android::hardware::graphics::common::V1_0::Dataspace;
using android::hardware::graphics::common::V1_0::PixelFormat;
using android::hardware::graphics::common::V1_0::Transform;
using GrallocError = android::hardware::graphics::mapper::V2_0::Error;

class GraphicsComposerHwcTest {
    public: 
    // returns an invalid display id (one that has not been registered to a
    // display.  Currently assuming that a device will never have close to
    // std::numeric_limit<uint64_t>::max() displays registered while running tests
    Display GetInvalidDisplayId() {
        std::vector<Display> validDisplays = mComposerCallback->getDisplays();
        uint64_t id = std::numeric_limits<uint64_t>::max();
        while (id > 0) {
            if (std::find(validDisplays.begin(), validDisplays.end(), id) == validDisplays.end()) {
                return id;
            }
            id--;
 	}

        return 0;
     }
	int  read_bmp(unsigned char **,char *);
    void sendRefreshFrame(char *filename);
    void GetClientTargetSupport();
    void SetPowerModeOff();
    void SetPowerModeOn();
    void set_color_transform();
    void CreateVirtualDisplay();
    void GetDisplayConfig();
    void SetActiveConfig(int);
    void set_layer_transform();
	void GetColorModes();
	void SetColorMode(int);
	void setLayerBuffer();
	void setLayerCompostionType();
      const native_handle_t* allocate() {
        uint64_t usage =
                static_cast<uint64_t>(BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN |
                                      BufferUsage::COMPOSER_OVERLAY);
        return mGralloc->allocate(mDisplayWidth, mDisplayHeight, 1, PixelFormat::RGBA_8888, usage);
    }
    void init();
    void execute() { mComposerClient->execute(mReader.get(), mWriter.get()); }

    std::unique_ptr<CommandWriterBase> mWriter;
    std::unique_ptr<TestCommandReader> mReader;
    int32_t mDisplayWidth = 640;
    int32_t mDisplayHeight = 480;

    private:
     std::unique_ptr<Gralloc> mGralloc;


    // use the slot count usually set by SF
    static constexpr uint32_t kBufferSlotCount = 64;

    std::unique_ptr<Composer> mComposer;
    std::unique_ptr<ComposerClient> mComposerClient;
    sp<GraphicsComposerCallback> mComposerCallback;
    // the first display and is assumed never to be removed
    Display mPrimaryDisplay;
    Display mInvalidDisplayId;

   private:
    Display waitForFirstDisplay() {
        while (true) {
            std::vector<Display> displays = mComposerCallback->getDisplays();
            if (displays.empty()) {
                usleep(5 * 1000);
                continue;
            }

            return displays[0];
        }
    }
};
void GraphicsComposerHwcTest::init() {

        mComposer = std::make_unique<Composer>(IComposer::getService("default"));
        mComposerClient = mComposer->createClient();

        mComposerCallback = new GraphicsComposerCallback;
        mComposerClient->registerCallback(mComposerCallback);

        // assume the first display is primary and is never removed
        mPrimaryDisplay = waitForFirstDisplay();

        // explicitly disable vsync
        mComposerClient->setVsyncEnabled(mPrimaryDisplay, false);
        mComposerCallback->setVsyncAllowed(false);

        mInvalidDisplayId = GetInvalidDisplayId();
        mWriter = std::make_unique<CommandWriterBase>(1024);
        mReader = std::make_unique<TestCommandReader>();
        ASSERT_NO_FATAL_FAILURE(mGralloc = std::make_unique<Gralloc>());
		std::vector<Config> configs = mComposerClient->getDisplayConfigs(mPrimaryDisplay);
		for (auto config : configs) {
        mDisplayWidth = mComposerClient->getDisplayAttribute(mPrimaryDisplay, config,
                                                             IComposerClient::Attribute::WIDTH);
        mDisplayHeight = mComposerClient->getDisplayAttribute(mPrimaryDisplay, config,
                                                              IComposerClient::Attribute::HEIGHT);
		}
        // Although 0 could be an invalid display, a return value of 0
        // from GetInvalidDisplayId means all other ids are in use, a condition which
        // we are assuming a device will never have
        //ASSERT_NE(0, mInvalidDisplayId);
}

typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef signed int LONG;
typedef unsigned char BYTE;
typedef long int UINT;
typedef struct tagBITMAPFILEHEADER {
    WORD   bfType;           /* "BM" */
    DWORD  bfSize;           /* Size of file in bytes */
    WORD   bfReserved1;      /* set to 0 */
    WORD   bfReserved2;      /* set to 0 */
    DWORD  bfOffBits;        /* Byte offset to actual bitmap data (= 54 if RGB) */
}__attribute__((packed)) BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER {
    DWORD  biSize;           /* Size of BITMAPINFOHEADER, in bytes (= 40) */
    LONG   biWidth;          /* Width of image, in pixels */
    LONG   biHeight;         /* Height of images, in pixels */
    WORD   biPlanes;         /* Number of planes in target device (set to 1) */
    WORD   biBitCount;       /* Bits per pixel (24 in this case) */
    DWORD  biCompression;    /* Type of compression (0 if no compression) */
    DWORD  biSizeImage;      /* Image size, in bytes (0 if no compression) */
    LONG   biXPelsPerMeter;  /* Resolution in pixels/meter of display device */
    LONG   biYPelsPerMeter;  /* Resolution in pixels/meter of display device */
    DWORD  biClrUsed;        /* Number of colors in the color table (if 0, use
                                maximum allowed by biBitCount) */
    DWORD  biClrImportant;   /* Number of important colors.  If 0, all colors
                                are important */
}__attribute__((packed)) BITMAPINFOHEADER;
int GraphicsComposerHwcTest::read_bmp(unsigned char** image,char* fileName){
	FILE* input;
    int width, height, level;
    BITMAPFILEHEADER bmfhdr;
    BITMAPINFOHEADER bmihdr;
	if (!(input = fopen(fileName, "r"))) {
	//if (!(input = fopen("/data/local/tmp/1.bmp", "r"))) {
            printf("Cannot open file .\n");
			return 0;
     }
	    fread(&bmfhdr.bfType, sizeof(WORD), 1, input);
        fread(&bmfhdr.bfSize, sizeof(DWORD), 1, input);
        fread(&bmfhdr.bfReserved1, sizeof(WORD), 1, input);
        fread(&bmfhdr.bfReserved2, sizeof(WORD), 1, input);
        fread(&bmfhdr.bfOffBits, sizeof(DWORD), 1, input);
        fread(&bmihdr.biSize, sizeof(DWORD), 1, input);
        fread(&bmihdr.biWidth, sizeof(LONG), 1, input);
        fread(&bmihdr.biHeight, sizeof(LONG), 1, input);
        fread(&bmihdr.biPlanes, sizeof(WORD), 1, input);
        fread(&bmihdr.biBitCount, sizeof(WORD), 1, input);
        fread(&bmihdr.biCompression, sizeof(DWORD), 1, input);
        fread(&bmihdr.biSizeImage, sizeof(DWORD), 1, input);
        fread(&bmihdr.biXPelsPerMeter, sizeof(LONG), 1, input);
        fread(&bmihdr.biYPelsPerMeter, sizeof(LONG), 1, input);
        fread(&bmihdr.biClrUsed, sizeof(DWORD), 1, input);
        fread(&bmihdr.biClrImportant, sizeof(DWORD), 1, input);
       // printf("+------------------+\n");
       // printf("| bfType: %d\n", bmfhdr.bfType);
       // printf("| bfSize: %d\n", bmfhdr.bfSize);
       // printf("| bfReserved1: %d\n", bmfhdr.bfReserved1);
       // printf("| bfReserved2: %d\n", bmfhdr.bfReserved2);
       // printf("| bfOffBits: %d\n", bmfhdr.bfOffBits);
       // printf("| biSize: %d\n", bmihdr.biSize);
       printf("| biWidth: %d\n", bmihdr.biWidth);
       printf("| biHeight: %d\n", bmihdr.biHeight);
       // printf("| biPlanes: %d\n", bmihdr.biPlanes);
       // printf("| biBitCount: %d\n", bmihdr.biBitCount);
       // printf("| biCompression: %d\n", bmihdr.biCompression);
       // printf("| biSizeImage: %d\n", bmihdr.biSizeImage);
       // printf("| biXPelsPerMeter: %d\n", bmihdr.biXPelsPerMeter);
       // printf("| biYPelsPerMeter: %d\n", bmihdr.biYPelsPerMeter);
       // printf("| biClrUsed: %d\n", bmihdr.biClrUsed);
       // printf("| biClrImportant: %d\n", bmihdr.biClrImportant);
       // printf("+------------------+\n");
        /* width and height of bmp image. */
        width = bmihdr.biWidth;
        height = bmihdr.biHeight;
		if(mDisplayWidth != width || mDisplayHeight != height){
			printf("fail to diplay picture\n");
			printf( "picture %d*%d size is not match display %d*%dsize ",width,height,mDisplayWidth,mDisplayHeight);
			return 0;
		}
        level = 4;
//		*out_width = width;
//		*out_height = height;
     //   *image = (BYTE*)malloc(height * width * level * sizeof(BYTE));
     //   memset(*image, 0x00, height * width * level * sizeof(BYTE));
        /* read pixel from file to array image. */
        /*
          bmihdr.biBitCount = 1  -> Binary image.
          bmihdr.biBitCount = 8  -> Gray-Level or Indexed image.
          bmihdr.biBitCount = 24 -> RGB image.
        */
        if ((bmihdr.biBitCount == 1) || (bmihdr.biBitCount == 8)) {
			return 0;
			/*
            offset = bmfhdr.bfOffBits - 54;
            fseek(input, offset, SEEK_CUR);
            auto size = height* width* bmihdr.biBitCount / 8;
            fread(image, 1, size, input);
            fclose(input);
			*///libavif did not surport this format
        }
    	else if (bmihdr.biBitCount == 24 || bmihdr.biBitCount == 32) {
        	int readPos = 0;
        	for (int i = height - 1; i >= 0; i--) {
           	 	readPos = i * width * (bmihdr.biBitCount / 8);
            	int s = (size_t)(long)width * (bmihdr.biBitCount / 8);
	//	std::cout << "readPos = " << readPos << std::endl;
	//	std::cout << s << std::endl;
            	 fread(*image + readPos, 1, s, input);
       		/*for (int i = 0; i < 1080; i++) {
           	 	readPos = i * width * (bmihdr.biBitCount / 8) + 138;
            	int s = (size_t)(long)width * (bmihdr.biBitCount / 8);
		std::cout << "readPos = " << readPos << std::endl;
		std::cout << s << std::endl;
            	 fread(*image + readPos, 1, s, input);
		 */
        }
        fclose(input);
    	}
        else {
            printf("Something wrong with biBitCount of image\n");
            return 0;
        }
	//	*bitCount = bmihdr.biBitCount;
		return 1;
}
void GraphicsComposerHwcTest::sendRefreshFrame(char * filename) {
     mWriter->selectDisplay(mPrimaryDisplay);
     mComposerClient->setPowerMode(mPrimaryDisplay, IComposerClient::PowerMode::ON);
     mComposerClient->setColorMode(mPrimaryDisplay, ColorMode::NATIVE);
     const native_handle_t* handle;
 
     ASSERT_NO_FATAL_FAILURE(handle = allocate());
     ASSERT_NE(nullptr, handle);
     const android::hardware::graphics::composer::V2_1::vts::AccessRegion region{0, 0, static_cast<int32_t>(mDisplayWidth),
                                static_cast<int32_t>(mDisplayHeight)};
     base::unique_fd fence;
     uint8_t* data;
     IComposerClient::Rect displayFrame{0, 0,mDisplayWidth, mDisplayHeight};
     std::cout << "start to display ..." << std::endl;
     data = static_cast<uint8_t*>(mGralloc->lock(handle, static_cast<uint64_t>(BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN),
                                                                         region, fence.release()));
	 if( read_bmp(&data,filename)!= 1){
	 std::cout << filename << std::endl;
     for (uint32_t x = 0; x < mDisplayWidth; x++) {
     for (uint32_t y = 0; y < mDisplayHeight; y++) {
             memset(data,0x00, 1);
	     data += 1;
             memset(data,0xf1, 1);
	     data += 1;
             memset(data,0x88, 1);
	     data += 1;
             memset(data,0xff, 1);
	     data += 1;
 
     }}
	 }
     Layer layer;
             layer = mComposerClient->createLayer(mPrimaryDisplay, kBufferSlotCount);
     mWriter->selectLayer(layer);
     mWriter->setLayerCompositionType(IComposerClient::Composition::DEVICE);
     mWriter->setLayerDisplayFrame(displayFrame);
     mWriter->setLayerPlaneAlpha(1);
     mWriter->setLayerSourceCrop({0, 0, (float)mDisplayWidth , (float)mDisplayHeight });
     mWriter->setLayerTransform(static_cast<Transform>(0));
     mWriter->setLayerVisibleRegion(std::vector<IComposerClient::Rect>(1, displayFrame));
     mWriter->setLayerZOrder(10);
     mWriter->setLayerBlendMode(IComposerClient::BlendMode::NONE);
     mWriter->setLayerSurfaceDamage(std::vector<IComposerClient::Rect>(1, displayFrame));
     mWriter->setLayerBuffer(0, handle, -1);
     mWriter->setLayerDataspace(Dataspace::UNKNOWN);
 
     mWriter->validateDisplay();
     execute();
     ASSERT_EQ(0, mReader->mErrors.size());
     mReader->mCompositionChanges.clear();
 
     mWriter->presentDisplay();
     execute();
	// sleep(5);
	 //ALOGD("1111 add  by xhw");
   // mWriter->setLayerTransform(static_cast<Transform>(0));
    // mWriter->setLayerTransform(Transform::ROT_90);
    // execute();
    // ASSERT_EQ(0, mReader->mErrors.size());
    // mReader->mCompositionChanges.clear();
 
    // mWriter->presentDisplay();
    // execute();
	// ALOGD("22222 add by xhw");
    // sleep(8);
   // mWriter->setLayerTransform(Transform::FLIP_V);
   // execute();
   // sleep(3);
   // mWriter->setLayerTransform(Transform::ROT_90);
    // layer.setLayerTransform(static_cast<Transform>(1));
   // execute();
   // sleep(30);
   // mWriter->presentDisplay();
   // sleep(3);
   // mWriter->setLayerTransform(Transform::ROT_180);
   // execute();
   // sleep(3);
 
 }

void GraphicsComposerHwcTest::GetClientTargetSupport() {
    std::vector<Config> configs = mComposerClient->getDisplayConfigs(mPrimaryDisplay);
    for (auto config : configs) {
        int32_t width = mComposerClient->getDisplayAttribute(mPrimaryDisplay, config,
                                                             IComposerClient::Attribute::WIDTH);
        int32_t height = mComposerClient->getDisplayAttribute(mPrimaryDisplay, config,
                                                              IComposerClient::Attribute::HEIGHT);
        ASSERT_LT(0, width);
        ASSERT_LT(0, height);
	std::cout << "display config = " <<config << std::endl;
	std::cout << "display width = " <<width << std::endl;
	std::cout << "display height = " << height << std::endl;
   // mComposerClient->setActiveConfig(mPrimaryDisplay, 1);
	//width = mComposerClient->getDisplayAttribute(mPrimaryDisplay, 1,
        //                                                     IComposerClient::Attribute::WIDTH);
	//std::cout << "width = " <<width << std::endl;
        //ASSERT_TRUE(mComposerClient->getClientTargetSupport(
         //   mPrimaryDisplay, width, height, PixelFormat::RGBA_8888, Dataspace::UNKNOWN));
		 return;
    }
}
void GraphicsComposerHwcTest::SetPowerModeOff() {
    std::vector<IComposerClient::PowerMode> modes;
    modes.push_back(IComposerClient::PowerMode::OFF);
   // modes.push_back(IComposerClient::PowerMode::ON);
   // modes.push_back(IComposerClient::PowerMode::OFF);
    for (auto mode : modes) {
        ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
    }
}
void  GraphicsComposerHwcTest::CreateVirtualDisplay() {
    if (mComposerClient->getMaxVirtualDisplayCount() == 0) {
        GTEST_SUCCEED() << "no virtual display support";
        return;
    }

    Display display;
    PixelFormat format;
    ASSERT_NO_FATAL_FAILURE(
        display = mComposerClient->createVirtualDisplay(64, 64, PixelFormat::IMPLEMENTATION_DEFINED,
                                                        kBufferSlotCount, &format));

    // test display type
    IComposerClient::DisplayType type = mComposerClient->getDisplayType(display);
    EXPECT_EQ(IComposerClient::DisplayType::VIRTUAL, type);

    //mComposerClient->destroyVirtualDisplay(display);
}

void GraphicsComposerHwcTest::SetPowerModeOn() {
    std::vector<IComposerClient::PowerMode> modes;
    modes.push_back(IComposerClient::PowerMode::ON);
   // modes.push_back(IComposerClient::PowerMode::ON);
   // modes.push_back(IComposerClient::PowerMode::OFF);
    for (auto mode : modes) {
        ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
    }
}
void GraphicsComposerHwcTest::GetDisplayConfig() {
    std::vector<Config> configs;
    ASSERT_NO_FATAL_FAILURE(configs = mComposerClient->getDisplayConfigs(mPrimaryDisplay));

    for (auto config : configs) {
    	std::cout << "display mode  = " << config << std::endl;
    }
}
void GraphicsComposerHwcTest::SetActiveConfig(int config) {
   // std::vector<Config> configs = mComposerClient->getDisplayConfigs(mPrimaryDisplay);
   // for (auto config : configs) {
        mComposerClient->setActiveConfig(mPrimaryDisplay, config);
     //   ASSERT_EQ(config, mComposerClient->getActiveConfig(mPrimaryDisplay));
   // }
}

//TEST_P(GraphicsComposerHidlCommandTest, SET_COLOR_TRANSFORM) {
void GraphicsComposerHwcTest::set_color_transform(){

		ALOGD("start setcolorTransfrom add by xhw");
    const std::array<float, 16> identity = {{
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f,
    }};

    mWriter->selectDisplay(mPrimaryDisplay);
    mWriter->setColorTransform(identity.data(), ColorTransform::IDENTITY);

    execute();
}

void GraphicsComposerHwcTest::set_layer_transform() {
    Layer layer;
	ALOGD("start setLayerTransfrom add by xhw");
    ASSERT_NO_FATAL_FAILURE(layer =
                                mComposerClient->createLayer(mPrimaryDisplay, kBufferSlotCount));

    mWriter->selectDisplay(mPrimaryDisplay);
    mWriter->selectLayer(layer);
    mWriter->setLayerTransform(static_cast<Transform>(0));
    mWriter->setLayerTransform(Transform::FLIP_H);
    execute();
    sleep(3);
    mWriter->setLayerTransform(Transform::FLIP_V);
    execute();
    sleep(3);
    mWriter->setLayerTransform(Transform::ROT_90);
    execute();
    sleep(3);
    mWriter->setLayerTransform(Transform::ROT_180);
    execute();
    sleep(3);
    mWriter->setLayerTransform(Transform::ROT_270);
    execute();
    sleep(3);
    mWriter->setLayerTransform(static_cast<Transform>(Transform::FLIP_H | Transform::ROT_90));
    execute();
    sleep(3);
    mWriter->setLayerTransform(static_cast<Transform>(Transform::FLIP_V | Transform::ROT_90));
    execute();
}

void GraphicsComposerHwcTest::GetColorModes() {
     std::vector<ColorMode> modes = mComposerClient->getColorModes(mPrimaryDisplay);
     auto nativeModeLocation = std::find(modes.begin(), modes.end(), ColorMode::NATIVE);
	 for(auto mode:modes){
		std::cout <<" color mode = "<< (int)mode << std::endl;
	 }
     //auto nativeModeLocation = std::find(modes.begin(), modes.end(), ColorMode::SRGB); 
     ASSERT_NE(modes.end(), nativeModeLocation);
 }
void GraphicsComposerHwcTest::SetColorMode(int mode) {
     /*std::unordered_set<ColorMode> validModes;
     for (auto mode : hidl_enum_range<ColorMode>()) {
         validModes.insert(mode);
     }
 
     std::vector<ColorMode> modes = mComposerClient->getColorModes(mPrimaryDisplay);
     for (auto mode : modes) {
	//	 std::cout <<" mode = " << ColorMode::NATIVE << std::endl;
         if (validModes.count(mode)) {
             mComposerClient->setColorMode(mPrimaryDisplay, mode);
         }
     }*/
     mComposerClient->setColorMode(mPrimaryDisplay, (ColorMode)mode);
 }
void GraphicsComposerHwcTest::setLayerBuffer() {  
     auto handle = allocate();  
     ASSERT_NE(nullptr, handle);  
  
     Layer layer;  
     ASSERT_NO_FATAL_FAILURE(layer =  
                                 mComposerClient->createLayer(mPrimaryDisplay, kBufferSlotCount));  
  
     mWriter->selectDisplay(mPrimaryDisplay);  
     mWriter->selectLayer(layer);  
     mWriter->setLayerBuffer(0, handle, -1);  
     execute();  
 }
void GraphicsComposerHwcTest::setLayerCompostionType() {
     Layer layer;
     ASSERT_NO_FATAL_FAILURE(layer =
                                 mComposerClient->createLayer(mPrimaryDisplay, kBufferSlotCount));

     mWriter->selectDisplay(mPrimaryDisplay);
     mWriter->selectLayer(layer);
     mWriter->setLayerCompositionType(IComposerClient::Composition::CLIENT);
     mWriter->setLayerCompositionType(IComposerClient::Composition::DEVICE);
     mWriter->setLayerCompositionType(IComposerClient::Composition::SOLID_COLOR);
     mWriter->setLayerCompositionType(IComposerClient::Composition::CURSOR);
     execute();
 }

}}}}}}
/*
int main(){
	//	std::unique_ptr<Composer> mComposer;
        //	std::unique_ptr<ComposerClient> mComposerClient;
       // 	mComposerClient = mComposer->createClient();
		android::hardware::graphics::composer::V2_1::vts::GraphicsComposerHwcTest mHwc;
		mHwc.init();
		mHwc.sendRefreshFrame();
		return 0;
}*/
