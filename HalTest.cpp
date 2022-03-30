#include "HalTest.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_PATH_SIZE 50
#define COMMAND "service call SurfaceFlinger 1035 i32 "
namespace android {namespace hardware {namespace graphics {
namespace composer {namespace V2_1 {namespace vts {
void GraphicsComposerHwcTest::getDisplayProperty() {
    std::vector<Config> configs = mComposerClient->getDisplayConfigs(mPrimaryDisplay);
    for (auto config : configs) {
        int32_t width = mComposerClient->getDisplayAttribute(mPrimaryDisplay, config,
                                                             IComposerClient::Attribute::    WIDTH);
        int32_t height = mComposerClient->getDisplayAttribute(mPrimaryDisplay, config,
                                                              IComposerClient::              Attribute::HEIGHT);
    ASSERT_LT(0, width);
    ASSERT_LT(0, height);
    std::cout << "display config = " <<config << std::endl;
    std::cout << "display width = " <<width << std::endl;
    std::cout << "display height = " << height << std::endl;
    return;
    }
}
void GraphicsComposerHwcTest::setPowerModeOff() {
    std::vector<IComposerClient::PowerMode> modes;
    modes.push_back(IComposerClient::PowerMode::OFF);
    for (auto mode : modes) {
        ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
    }
}
void  GraphicsComposerHwcTest::createVirtualDisplay() {
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

    mComposerClient->destroyVirtualDisplay(display);
}
void GraphicsComposerHwcTest::setPowerModeOn() {
    std::vector<IComposerClient::PowerMode> modes;
    modes.push_back(IComposerClient::PowerMode::ON);
    for (auto mode : modes) {
        ASSERT_NO_FATAL_FAILURE(mComposerClient->setPowerMode(mPrimaryDisplay, mode));
    }
}
void GraphicsComposerHwcTest::getDisplayConfig() {
    std::vector<Config> configs;
    ASSERT_NO_FATAL_FAILURE(configs = mComposerClient->getDisplayConfigs(mPrimaryDisplay));

    for (auto config : configs) {
        std::cout << "display mode  = " << config << std::endl;
    }
}
void GraphicsComposerHwcTest::setActiveConfig(int config) {
        mComposerClient->setActiveConfig(mPrimaryDisplay, config);
}

void GraphicsComposerHwcTest::setColorTransform(){

    const std::array<float, 16> identity = {{
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f,
    }};

    mWriter->selectDisplay(mPrimaryDisplay);
    mWriter->setColorTransform(identity.data(), ColorTransform::IDENTITY);

    execute();
}
void GraphicsComposerHwcTest::setLayerTransform() {
    Layer layer;
    ASSERT_NO_FATAL_FAILURE(layer =mComposerClient->createLayer(mPrimaryDisplay,kBufferSlotCount));
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
void GraphicsComposerHwcTest::getColorModes() {
     std::vector<ColorMode> modes = mComposerClient->getColorModes(mPrimaryDisplay);
     auto nativeModeLocation = std::find(modes.begin(), modes.end(), ColorMode::NATIVE);
     for(auto mode:modes){
        std::cout <<" color mode = "<< (int)mode << std::endl;
     }
     ASSERT_NE(modes.end(), nativeModeLocation);
 }
void GraphicsComposerHwcTest::setColorMode(int mode) {
     mComposerClient->setColorMode(mPrimaryDisplay, (ColorMode)mode);
 }
void GraphicsComposerHwcTest::setLayerBuffer() {
     auto handle = allocate();
     ASSERT_NE(nullptr, handle);

     Layer layer;
     ASSERT_NO_FATAL_FAILURE(layer =mComposerClient->createLayer(mPrimaryDisplay,kBufferSlotCount));

     mWriter->selectDisplay(mPrimaryDisplay);
     mWriter->selectLayer(layer);
     mWriter->setLayerBuffer(0, handle, -1);
     execute();
 }
void GraphicsComposerHwcTest::setLayerCompostionType() {
     Layer layer;
     ASSERT_NO_FATAL_FAILURE(layer =mComposerClient->createLayer(mPrimaryDisplay,kBufferSlotCount));

     mWriter->selectDisplay(mPrimaryDisplay);
     mWriter->selectLayer(layer);
     mWriter->setLayerCompositionType(IComposerClient::Composition::CLIENT);
     mWriter->setLayerCompositionType(IComposerClient::Composition::DEVICE);
     mWriter->setLayerCompositionType(IComposerClient::Composition::SOLID_COLOR);
     mWriter->setLayerCompositionType(IComposerClient::Composition::CURSOR);
     execute();
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
     WORD   biPlanes;        /* Number of planes in target device (set to 1) */
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
int GraphicsComposerHwcTest::readBmp(unsigned char** image,char* fileName){
	FILE* input;
	int width, height, level;
	BITMAPFILEHEADER bmfhdr;
	BITMAPINFOHEADER bmihdr;
	if (!(input = fopen(fileName, "r"))) {
        printf("Cannot open file %s.\n",fileName);
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
    printf("| biWidth: %d\n", bmihdr.biWidth);
    printf("| biHeight: %d\n", bmihdr.biHeight);
    width = bmihdr.biWidth;
    height = bmihdr.biHeight;
    if(mDisplayWidth != width || mDisplayHeight != height){
        printf("fail to diplay picture %s\n",fileName);
        printf( "picture %d*%d size is not match display %d*%dsize ",width,height,       mDisplayWidth,mDisplayHeight);
        return 0;
    }
    level = 4;
    if ((bmihdr.biBitCount == 1) || (bmihdr.biBitCount == 8)) {
        return 0;
    }
    else if (bmihdr.biBitCount == 24 || bmihdr.biBitCount == 32) {
        int readPos = 0;
        for (int i = height - 1; i >= 0; i--) {
            readPos = i * width * (bmihdr.biBitCount / 8);
            int s = (size_t)(long)width * (bmihdr.biBitCount / 8);
             fread(*image + readPos, 1, s, input);
    }
    fclose(input);
    }
    else {
        printf("Something wrong with biBitCount of image\n");
        return 0;
    }
    return 1;
}
void GraphicsComposerHwcTest::displayBmpPicture(char * filename) {
    mWriter->selectDisplay(mPrimaryDisplay);
    mComposerClient->setPowerMode(mPrimaryDisplay, IComposerClient::PowerMode::ON);
    mComposerClient->setColorMode(mPrimaryDisplay, ColorMode::NATIVE);
    const native_handle_t* handle;

    ASSERT_NO_FATAL_FAILURE(handle = allocate());
    ASSERT_NE(nullptr, handle);
    const android::hardware::graphics::composer::V2_1::vts::AccessRegion region{0, 0,       static_cast<int32_t>(mDisplayWidth),
                               static_cast<int32_t>(mDisplayHeight)};
    base::unique_fd fence;
    uint8_t* data;
    IComposerClient::Rect displayFrame{0, 0,mDisplayWidth, mDisplayHeight};
    std::cout << "start to display ..." << std::endl;
    data = static_cast<uint8_t*>(mGralloc->lock(handle,static_cast<uint64_t>(BufferUsage::CPU_WRITE_OFTEN | BufferUsage::CPU_READ_OFTEN),
                                                                        region, fence. release()));
    if( readBmp(&data,filename)!= 1){
	//If the picture display fails, fill the memory with all colors
   // std::cout << filename << std::endl;
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

}

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
                                                         IComposerClient::Attribute::    WIDTH);
    mDisplayHeight = mComposerClient->getDisplayAttribute(mPrimaryDisplay, config,
                                                          IComposerClient::              Attribute::HEIGHT);
    }
}
}}}}}}

int main(int argc,char *argv[]){
	if(argc < 1){
		printf("error: Too few parameters");
		return -1;
	}
	int ch;
	opterr = 0;
	char *filePath = (char *)malloc(MAX_PATH_SIZE);
	char *command  = (char *)malloc(MAX_PATH_SIZE);
	if(strcmp(argv[1],"-s") != 0 && strcmp(argv[2],"mode")!=0){
		system("stop surfaceflinger");
		android::hardware::graphics::composer::V2_1::vts::GraphicsComposerHwcTest mHwc;
		mHwc.init();
		while((ch = getopt(argc,argv,"a:bcdesg"))!= -1){
			switch(ch){
			case 's'://set
			if(strcmp(argv[2],"power") == 0 && strcmp(argv[3],"on") == 0){
				mHwc.setPowerModeOn();
			}
			else if(strcmp(argv[2],"power") == 0 && strcmp(argv[3],"off") == 0){
				mHwc.setPowerModeOff();
			}
			else if(strcmp(argv[2],"display") == 0 &&strcmp(argv[3],"mode") == 0 ){
				mHwc.setActiveConfig(atoi(argv[3]));
    	    		}else if(strcmp(argv[2],"color") == 0 && strcmp(argv[3],"mode") == 0){
				mHwc.setColorMode(atoi(argv[4]));
			
    	    		}else if(strcmp(argv[2],"layer") == 0 && strcmp(argv[3],"buffer") == 0){
				mHwc.setLayerBuffer();
			}
			else if(strcmp(argv[2],"layer") == 0 && strcmp(argv[3],"transform") == 0){
				std::cout << "layer transform " << std::endl;
				mHwc.setLayerTransform();
			}else if(strcmp(argv[2],"color") == 0 && strcmp(argv[3],"transform") == 0){
    	        		std::cout << "color transform" << std::endl;
    	       	 		mHwc.setColorTransform();
			}else if(strcmp(argv[2],"layer") == 0 && strcmp(argv[3],"compostion") == 0){
				mHwc.setLayerCompostionType();
			}
			break;
			case 'c'://create
			if(strcmp(argv[2],"VirtualDisplay") == 0 ){
				mHwc.createVirtualDisplay();
			}
			break;
			case 'd'://display
			strcpy(filePath, argv[2]);
    	    		mHwc.displayBmpPicture(filePath);
			break;
			case 'g'://get
			if(strcmp(argv[2],"color") == 0&& strcmp(argv[3],"mode")==0 ){
				mHwc.getColorModes();
			}else 
			if(strcmp(argv[2],"display") == 0 && strcmp(argv[3],"mode") == 0){
				mHwc.getDisplayConfig();
			}else 
			if(strcmp(argv[2],"display") == 0 && strcmp(argv[3],"property") == 0 ){	
				mHwc.getDisplayProperty();
			}
			break;
			}
		}//end switch	
	}//end while
	else{
		strcat(command,COMMAND);
		strcat(command,argv[3]);
		printf("the command = %s\n",command);
		system(command);	
	}//end if
    return 0;
} 
