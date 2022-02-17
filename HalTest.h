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
    void sendRefreshFrame();
    void GetClientTargetSupport();
    void SetPowerModeOff();
    void SetPowerModeOn();
    void set_color_transform();
    void CreateVirtualDisplay();
    void GetDisplayConfig();
    void SetActiveConfig();
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
    int32_t mDisplayWidth = 1920;
    int32_t mDisplayHeight = 1080;

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

        // Although 0 could be an invalid display, a return value of 0
        // from GetInvalidDisplayId means all other ids are in use, a condition which
        // we are assuming a device will never have
        //ASSERT_NE(0, mInvalidDisplayId);
}
void GraphicsComposerHwcTest::sendRefreshFrame() {
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
     std::cout << "6 powerdown off" << std::endl;
     data = static_cast<uint8_t*>(mGralloc->lock(handle, static_cast<uint64_t>(BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN),
                                                                         region, fence.release()));
     for (uint32_t x = 0; x < mDisplayWidth; x++) {
     for (uint32_t y = 0; y < mDisplayHeight; y++) {
             memset(data,0x00, 1);
             memset(data,0xff, 1);
             memset(data,0xff, 1);
             memset(data,0x88, 1);
 
             data += 4;
     }}
 
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
 
     sleep(10);/* */
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
	std::cout << "config = " <<config << std::endl;
	std::cout << "width = " <<width << std::endl;
	std::cout << "height = " << height << std::endl;
        mComposerClient->setActiveConfig(mPrimaryDisplay, 1);
	//width = mComposerClient->getDisplayAttribute(mPrimaryDisplay, 1,
        //                                                     IComposerClient::Attribute::WIDTH);
	//std::cout << "width = " <<width << std::endl;
        //ASSERT_TRUE(mComposerClient->getClientTargetSupport(
         //   mPrimaryDisplay, width, height, PixelFormat::RGBA_8888, Dataspace::UNKNOWN));
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
/*    std::cout << "1...."  << std::endl;
    sleep(5);
    modes.clear();

    modes.push_back(IComposerClient::PowerMode::OFF);
    modes.push_back(IComposerClient::PowerMode::OFF);
    for (auto mode : modes) {
        ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
    }
    std::cout << "2...."  << std::endl;
    sleep(5);
    modes.clear();
    if (mComposerClient->getDozeSupport(mPrimaryDisplay)) {
        modes.push_back(IComposerClient::PowerMode::DOZE);
        modes.push_back(IComposerClient::PowerMode::DOZE);

        for (auto mode : modes) {
            ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
        }

        modes.clear();

        modes.push_back(IComposerClient::PowerMode::DOZE_SUSPEND);
        modes.push_back(IComposerClient::PowerMode::DOZE_SUSPEND);

        for (auto mode : modes) {
            ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
        }
    }
    std::cout << "3...."  << std::endl;
    sleep(5);

    modes.clear();

    modes.push_back(IComposerClient::PowerMode::ON);
    modes.push_back(IComposerClient::PowerMode::ON);
    for (auto mode : modes) {
        ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
    }
    std::cout << "4...." << std::endl;
    sleep(5);*/
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

    std::cout << "config  = "<< std::endl;
    for (auto config : configs) {
    	std::cout << "config  = " << config << std::endl;
    }
}
void GraphicsComposerHwcTest::SetActiveConfig() {
   // std::vector<Config> configs = mComposerClient->getDisplayConfigs(mPrimaryDisplay);
   // for (auto config : configs) {
        mComposerClient->setActiveConfig(mPrimaryDisplay, 7);
     //   ASSERT_EQ(config, mComposerClient->getActiveConfig(mPrimaryDisplay));
   // }
}

//TEST_P(GraphicsComposerHidlCommandTest, SET_COLOR_TRANSFORM) {
void GraphicsComposerHwcTest::set_color_transform(){
    const std::array<float, 16> identity = {{
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f,
    }};

    mWriter->selectDisplay(mPrimaryDisplay);
    mWriter->setColorTransform(identity.data(), ColorTransform::IDENTITY);

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
