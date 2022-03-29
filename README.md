hwc utility 功能介绍

1. 显示bmp格式图片
./hwc-utility -d image.bmp

2 控制电源
开：./hwc-utility -s power on                                                                                                             
关：./hwc-utility -s power off


3 控示显示模式
	1 查询能显示的mode                                                                                                              
		./hwc-utility -g display mode                                                                                               
	2 控制显示模式                                                                                                                       
		./hwc-utility -s display mode mode id                                                                                       
4 查看显示mode的property                                                                                                                     
./hwc-utility -g display property                                                                                                           

5 创建虚拟屏
./hwc-utility -c VirtualDisplay

6 控制layer 翻转
./hwc-utility -s layer transform                                                                                                         
设置Layer Buffer
./hwc-utility -s layer buffer

7 控制颜色翻转
./hwc-utility -s color transform

8 查询ColorMode
./hwc-utility -g color mode  //返回id为数值，可对照/hardware/interfaces/graphics/common/1.0/types.hal 查询

9 设置ColorMOde
./hwc-utility -s color mode id
