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
namespace android {namespace hardware {namespace graphics {
namespace composer {namespace V2_1 {namespace vts {
using android::hardware::graphics::common::V1_0::BufferUsage;
using android::hardware::graphics::common::V1_0::ColorMode;
using android::hardware::graphics::common::V1_0::ColorTransform;
using android::hardware::graphics::common::V1_0::Dataspace;
using android::hardware::graphics::common::V1_0::PixelFormat;
using android::hardware::graphics::common::V1_0::Transform;
using GrallocError = android::hardware::graphics::mapper::V2_0::Error;

class GraphicsComposerHwcTest {
public: 
    int32_t mDisplayWidth = 640;
    int32_t mDisplayHeight = 480;
    std::unique_ptr<CommandWriterBase> mWriter;
    std::unique_ptr<TestCommandReader> mReader;
    int  readBmp (unsigned char **,char *);
    void displayBmpPicture(char *filename);
    void displayStripePicture();
    void getDisplayProperty();
    void setPowerModeOff();
    void setPowerModeOn();
    void setColorTransform();
    void createVirtualDisplay();
    void getActiveConfig();
    void setActiveConfig(int);
    void setLayerTransform();
    void getColorModes();
    void setColorMode(int);
    void setLayerBuffer();
    void setLayerCompostionType();
    void init();
    const native_handle_t* allocate() {
    uint64_t usage =static_cast<uint64_t>(BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN |BufferUsage::COMPOSER_OVERLAY);
        return mGralloc->allocate(mDisplayWidth, mDisplayHeight, 1, PixelFormat::RGBA_8888, usage);
    }
    void execute() { 
	mComposerClient->execute(mReader.get(), mWriter.get()); 
    }
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
}}}}}}
