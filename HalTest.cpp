#include "HalTest.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_PATH_SIZE 50
#define COMMAND "service call SurfaceFlinger 1035 i32 "
int main(int argc,char *argv[]){
		if(argc < 1)
		return 0;
		int ch;
		opterr = 0;
		char *result = (char *)malloc(MAX_PATH_SIZE);
		char *command = (char *)malloc(MAX_PATH_SIZE);
		if(strcmp(argv[1],"-s") == 0 && strcmp(argv[2],"mode")==0){
			strcat(command,COMMAND);
			strcat(command,argv[3]);
			printf("the command = %s/n",command);
			system(command);
		}else {
		system("stop surfaceflinger");
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
			else if(strcmp(argv[2],"display") == 0 &&strcmp(argv[3],"mode") == 0 ){
				mHwc.SetActiveConfig(atoi(argv[3]));
			}else if(strcmp(argv[2],"layer") == 0 && strcmp(argv[3],"transfrom") == 0){
				std::cout << "layer" << std::endl;
				mHwc.set_layer_transform();
			}else if(strcmp(argv[2],"color") == 0 && strcmp(argv[3],"transfrom") == 0){
                                std::cout << "color" << std::endl;
                                mHwc.set_color_transform();
            }else if(strcmp(argv[2],"color") == 0 && strcmp(argv[3],"mode") == 0){
				mHwc.SetColorMode(atoi(argv[4]));
			
            }else if(strcmp(argv[2],"layer") == 0 && strcmp(argv[3],"buffer") == 0){
				mHwc.setLayerBuffer();
			}else if(strcmp(argv[2],"layer") == 0 && strcmp(argv[3],"compostion") == 0){
				mHwc.setLayerCompostionType();
			}
			break;
			case 'c':
			if(strcmp(argv[2],"VirtualDisplay") == 0 ){
				mHwc.CreateVirtualDisplay();
			}
		//	mHwc.SetPowerModeVariations();
			break;
			case 'd':
			strcpy(result, argv[2]);
            mHwc.sendRefreshFrame(result);
			break;
			case 'g':
		//	mHwc.set_color_transform();
			if(strcmp(argv[2],"display") == 0 && strcmp(argv[3],"mode") == 0){
				mHwc.GetDisplayConfig();
			}else if(strcmp(argv[2],"color") == 0&& strcmp(argv[3],"mode")==0 ){
				mHwc.GetColorModes();
			}else if(strcmp(argv[2],"display") == 0 && strcmp(argv[3],"property") == 0 ){	
				mHwc.GetClientTargetSupport();
			}
			break;
			}
		}//end while
		}//end if
                return 0;
}

