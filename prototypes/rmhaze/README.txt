目的
	应用dark channel实现单张图像的去雾操作。

大致算法流程
	1. 计算darkchannel图像
	2. 计算atmospheric light的值A
	3. 计算前背景估计系数t
	4. soft matting，细化t的边缘
	5. 复原图像

文件说明
	rmhaze.m
		主程序，在第9行指定要去雾的图像
	darkchannel.m
		计算darkchannel的函数
	

参考文献
	1. Kaiming He, Jian Sun, and Xiaoou Tang, "Single Image Haze Removal Using Dark Channel Prior".
	2. A. Levin, D. Lischinski, and Y. Weiss, "A Closed Form Solution to Natural Image Matting".






