#include "HalTest.h"
int main(){
                android::hardware::graphics::composer::V2_1::vts::GraphicsComposerHwcTest mHwc;
                mHwc.init();
                mHwc.sendRefreshFrame();
                return 0;
}

