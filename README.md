version 1.0                                                                                                                                                             
How to build                                                                                                                                                           
Copy this folder hwc-test to any location                                                                                              
under the Android source code                                                                                                             
Compile this program with the mm command

hwc utility Function introduction                                                                                                         
1
1.1 display stripe picture                                                                                                                                             
./hwc-utility -d stripe

1.2 Display BMP format pictures                                                                                                          
./hwc-utility -d picture image.bmp 

2 Control power supply                                                                                                                   
开：./hwc-utility -s power on                                                                                                             
关：./hwc-utility -s power off

3 control display mode                                                                                                                   
3.1 query the current active config                                                                                         
./hwc-utility -g active mode
 
3.2 Set display mode  
setprop vendor.hwcomposer.preferred.mode.limit  0                                                                                         
stop vendor.hwcomposer-2-4                                                                                                               
start vendor.hwcomposer-2-4                                                                                                        
stop;start                                                                                                              
./hwc-utility -s display mode id

 3.3 View the property of all display mode property                        
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
./hwc-utility -g color mode //返回id为数值，可对照/hardware/interfaces/graphics/common/1.0/types.hal 查询

9 set Color Mode                                                                                                                                                       
./hwc-utility -s color mode id
 
