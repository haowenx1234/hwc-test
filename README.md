hwc utility 功能介绍

1. 显示bmp格式图片
./hwc-utility -d image.bmp


2 控制电源
开：./hwc-utility -s power on
关：./hwc-utility -s power off


3 控示显示模式
	1 查询能显示的mode 和 特性
		./hwc-utility -g mode
		./hwc-untility -g modeid  WIDTH
	2 控制显示模式
		./hwc-utility -s mode mode_id（还在实现）

4 创建虚拟屏
  ./hwc-utility -c VirtualDisplay

5 控制layer 翻转

./hwc-utility -s layer transform

6 设置Layer Buffer

  设置Layer DATASPACR

  设置Layer DISPLAY_FRAME

  设置Layer PLANE_ALPHA

  设置Layer SOURCE_CROP

  设置Layer VISABLE_REGION

  设置Layer Z_ORDER

7 控制颜色翻转

./hwc-utility -s color transform



8 查询ColorMode

9 设置ColorMOde
