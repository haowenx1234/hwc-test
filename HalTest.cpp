#include "HalTest.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char *argv[]){
		if(argc < 1)
		return 0;
		int ch;
		opterr = 0;
                android::hardware::graphics::composer::V2_1::vts::GraphicsComposerHwcTest mHwc;
                mHwc.init();
		while((ch = getopt(argc,argv,"a:bcdesg"))!= -1){
			switch(ch){
			case 's':
			if(strcmp(argv[2],"power") == 0 && strcmp(argv[3],"on") == 0){
				mHwc.SetPowerModeOn();
			}
			else if(strcmp(argv[2],"power") == 0 && strcmp(argv[3],"off") == 0){
				mHwc.SetPowerModeOff();
			}
			else if(strcmp(argv[2],"mode") == 0 ){
				mHwc.SetActiveConfig();
			}

		//	mHwc.GetClientTargetSupport();
			break;
			case 'c':
			mHwc.CreateVirtualDisplay();
		//	mHwc.SetPowerModeVariations();
			break;
			case 'd':
                	mHwc.sendRefreshFrame();
			break;
			case 'g':
		//	mHwc.set_color_transform();
			mHwc.GetDisplayConfig();
			break;
			}
		}
                return 0;
}

