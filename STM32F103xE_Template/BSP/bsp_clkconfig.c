#include "stm32f10x.h"
#include "bsp_clkconfig.h"

/*
 * ʹ��HSEʱ������ϵͳʱ�ӵĲ���
 * 1������HSE �����ȴ� HSE �ȶ�
 * 2������ AHB��APB2��APB1��Ԥ��Ƶ����
 * 3������PLL��ʱ����Դ����PLL�ı�Ƶ���ӣ����ø���Ƶ����Ҫ��������������
 * 4������PLL�����ȴ�PLL�ȶ�
 * 5����PLLCK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 */

/* ���� ϵͳʱ��:SYSCLK, AHB����ʱ��:HCLK, APB2����ʱ��:PCLK2, APB1����ʱ��:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,���ֻ����36M
 * ����˵����pllmul��PLL�ı�Ƶ���ӣ��ڵ��õ�ʱ������ǣ�RCC_PLLMul_x , x:[2,3,...16]
 * ������User_SetSysClock(RCC_PLLMul_9);  ������ϵͳʱ��Ϊ��8MHZ * 9 = 72MHZ
 *       User_SetSysClock(RCC_PLLMul_16); ������ϵͳʱ��Ϊ��8MHZ * 16 = 128MHZ����Ƶ����
 *
 * HSE��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�����ͨ��������
 */
void HSE_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t StartUpCounter = 0, HSEStartUpStatus = 0;

    RCC_DeInit();// ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
    RCC_HSEConfig(RCC_HSE_ON);//ʹ��HSE�������ⲿ����Ұ�𿪷����õ���8M

    do
    {
        HSEStartUpStatus = RCC_WaitForHSEStartUp();
        StartUpCounter++;
    }
    while ((HSEStartUpStatus == 0) && (StartUpCounter != 9)); //�ȴ� HSE �����ȶ�

    // ֻ�� HSE �ȶ�֮�����������ִ��
    if (HSEStartUpStatus != 0)
    {
//----------------------------------------------------------------------//
        // ʹ��FLASH Ԥ��ȡ������
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
        // ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
        // ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
        // 0��0 < SYSCLK <= 24M
        // 1��24< SYSCLK <= 48M
        // 2��48< SYSCLK <= 72M
        FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//

        RCC_HCLKConfig(RCC_SYSCLK_Div1);// AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK
        RCC_PCLK2Config(RCC_HCLK_Div1); // APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
        RCC_PCLK1Config(RCC_HCLK_Div2);// APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2

//-----------------���ø���Ƶ����Ҫ��������������-------------------//
        // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
        // PLLCLK = 8MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pllmul);
//------------------------------------------------------------------//

        RCC_PLLCmd(ENABLE); // ����PLL
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);// �ȴ� PLL�ȶ�
        RCC_SYSCLKConfig(
            RCC_SYSCLKSource_PLLCLK);// ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK
        while (RCC_GetSYSCLKSource() != 0x08);// ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��

    }
    else
    {
        // ���HSE����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
        // ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
        // HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
        while (1)
        {
        }
    }
}

/*
 * ʹ��HSIʱ������ϵͳʱ�ӵĲ���
 * 1������HSI �����ȴ� HSI �ȶ�
 * 2������ AHB��APB2��APB1��Ԥ��Ƶ����
 * 3������PLL��ʱ����Դ����PLL�ı�Ƶ���ӣ����ø���Ƶ����Ҫ��������������
 * 4������PLL�����ȴ�PLL�ȶ�
 * 5����PLLCK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 */

/* ���� ϵͳʱ��:SYSCLK, AHB����ʱ��:HCLK, APB2����ʱ��:PCLK2, APB1����ʱ��:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2,���ֻ����36M
 * ����˵����pllmul��PLL�ı�Ƶ���ӣ��ڵ��õ�ʱ������ǣ�RCC_PLLMul_x , x:[2,3,...16]
 * ������HSI_SetSysClock(RCC_PLLMul_9);  ������ϵͳʱ��Ϊ��4MHZ * 9 = 72MHZ
 *       HSI_SetSysClock(RCC_PLLMul_16); ������ϵͳʱ��Ϊ��4MHZ * 16 = 64MHZ
 *
 * HSI��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�������HSE���ϵ�ʱ���ʹ�õķ���
 * HSI����Ϊ�¶ȵ�ԭ�����Ư�ƣ����ȶ���һ�㲻����HSI��Ϊʱ����Դ���������Ȳ����ѵ����
 * ���HSIҪ��ΪPLLʱ�ӵ���Դ�Ļ����������Ƶ֮��ſ��ԣ���HSI/2����PLL��Ƶ�������ֻ����16
 * ���Ե�ʹ��HSI��ʱ��SYSCLK���ֻ����4M*16=64M
 */

void HSI_SetSysClock(uint32_t pllmul)
{
    __IO uint32_t HSIStartUpStatus = 0;

    RCC_DeInit();// ��RCC�����ʼ���ɸ�λ״̬������Ǳ����
    RCC_HSICmd(ENABLE);//ʹ��HSI
    HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY; // �ȴ� HSI ����

    // ֻ�� HSI����֮�����������ִ��
    if (HSIStartUpStatus == RCC_CR_HSIRDY)
    {
//----------------------------------------------------------------------//
        // ʹ��FLASH Ԥ��ȡ������
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        // SYSCLK�������������ʱ��ı������ã�����ͳһ���ó�2
        // ���ó�2��ʱ��SYSCLK����48MҲ���Թ�����������ó�0����1��ʱ��
        // ������õ�SYSCLK�����˷�Χ�Ļ���������Ӳ�����󣬳��������
        // 0��0 < SYSCLK <= 24M
        // 1��24< SYSCLK <= 48M
        // 2��48< SYSCLK <= 72M
        FLASH_SetLatency(FLASH_Latency_2);
//----------------------------------------------------------------------//

        RCC_HCLKConfig(RCC_SYSCLK_Div1); // AHBԤ��Ƶ��������Ϊ1��Ƶ��HCLK = SYSCLK
        RCC_PCLK2Config(RCC_HCLK_Div1);// APB2Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK2 = HCLK
        RCC_PCLK1Config(RCC_HCLK_Div2);// APB1Ԥ��Ƶ��������Ϊ1��Ƶ��PCLK1 = HCLK/2

//-----------------���ø���Ƶ����Ҫ��������������-------------------//
        // ����PLLʱ����ԴΪHSE������PLL��Ƶ����
        // PLLCLK = 4MHz * pllmul
        RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
//------------------------------------------------------------------//

        RCC_PLLCmd(ENABLE);// ����PLL
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);// �ȴ� PLL�ȶ�
        RCC_SYSCLKConfig(
            RCC_SYSCLKSource_PLLCLK);// ��PLL�ȶ�֮�󣬰�PLLʱ���л�Ϊϵͳʱ��SYSCLK

        while (RCC_GetSYSCLKSource() != 0x08); // ��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��

    }
    else
    {
        // ���HSI����ʧ�ܣ���ô����ͻ���������û�����������ӳ���Ĵ��봦��
        // ��HSE����ʧ�ܻ��߹��ϵ�ʱ�򣬵�Ƭ�����Զ���HSI����Ϊϵͳʱ�ӣ�
        // HSI���ڲ��ĸ���ʱ�ӣ�8MHZ
        while (1)
        {
        }
    }
}

/*
 * ��ʼ��MCO����PA8
 * ��F1ϵ����MCO����ֻ��һ������PA8����F4ϵ���У�MCO���Ż�������
 */
void MCO_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // ����GPIOA��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // ѡ��GPIO8����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

    //����Ϊ���ù����������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

    //����IO�ķ�ת����Ϊ50M
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // ��ʼ��GPIOA8
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ����MCO�������ʱ�ӣ���ʾ����������PA8�����������ʱ���ź�
    // MCO�������������HSE,HSI,PLLCLK/2,SYSCLK
    //RCC_MCOConfig(RCC_MCO_HSE);                // 8M
    //RCC_MCOConfig(RCC_MCO_HSI);              // 8M
    //RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);    // 36M
    RCC_MCOConfig(RCC_MCO_SYSCLK);           // 72M
}
