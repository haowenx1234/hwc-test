How to build                                                                                                                                                           
Copy this folder hwc-test to any location 
under the Android source code
Compile this program with the mm command

hwc utility Function introduction                                                                                                                                       

1.Display BMP format pictures
./hwc-utility -d image.bmp

2 Control power supply                                                                                                                                                 
开：./hwc-utility -s power on                                                                                                             
关：./hwc-utility -s power off


3 control display mode                                                                                                                                                 
 3.1 query the mode that can be displayed                                                                                          
 ./hwc-utility -g display mode                                                                                               
 3.2 Set display mode                                                                                                         
 ./hwc-utility -s display mode mode id                                                                                       
 3.3 View the property of the display mode                                                                                                                     
./hwc-utility -g display property                                                                                                           

5 Create virtual screen
./hwc-utility -c VirtualDisplay

6 Control layer transform
./hwc-utility -s layer transform                                                                                                         
set Layer Buffer
./hwc-utility -s layer buffer

7 set color tarnsform
./hwc-utility -s color transform

8  querry ColorMode
./hwc-utility -g color mode  //返回id为数值，可对照/hardware/interfaces/graphics/common/1.0/types.hal 查询

9 set ColorMOde
./hwc-utility -s color mode id
