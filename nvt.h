#ifndef __NVT_H__
#define __NVT_H__

#define NVT_BASE 235
#define NVT_EOF	236	
//文件结束符
#define NVT_SUSP	237	
//挂起当前进程
#define NVT_ABORT	238	
//中止进程
#define NVT_EOR	239	
//记录结束符
#define NVT_SE	240	
//子选项结束
#define NVT_NOP	241	
//空操作
#define NVT_DM	242	
//数据标记
#define NVT_BRK	243	
//终止符（break）
#define NVT_IP	244	
//终止进程
#define NVT_AO	245	
//终止输出
#define NVT_AYT	246	
//请求应答
#define NVT_EC	247	
//终止符
#define NVT_EL	248	
//擦除一行
#define NVT_GA	249	
//继续
#define NVT_SB	250	
//子选项开始
#define NVT_WILL	251	
//选项协商
#define NVT_WONT	252	
//选项协商
#define NVT_DO	253	
//选项协商
#define NVT_DONT	254	
//选项协商
#define NVT_IAC	255	
//字符0XFF
#endif

