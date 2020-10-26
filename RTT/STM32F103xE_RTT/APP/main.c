


//定义线程栈
ALIGN(RT_ALIGN_SIZE)
rt_uint8_t rt_flag1_thread_satck[512];
rt_uint8_t rt_flag2_thread_satck[512];

//定义线程控制块
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;

//定义变量
rt_uint8_t flag1=0,flag2=0;

/**
 * @Brief : 软件延时
 * @param  count            
 */
void delay(rt_uint32_t count)
{
	for (; count!=0; cpunt--);
}

//线程1主体
void flag1_thread_entery(void *Pragma)
{
	for (;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
	}
	
}

//线程2主体
void flag1_thread_entery(void *Pragma)
{
	for (;;)
	{
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
	}
	
}


int main(void)
{	
	
	while (1)
	{
		
	}
}

