//-------------------------------------包含相关头文件-----------------------------------//
#include <cyg/infra/testcase.h> // 测试宏定义,如 CYG_TEST_FAIL_FINISH
#include <cyg/io/io.h> // 声明 I/O 子系统 API 函数
#include <cyg/kernel/kapi.h> // 定义针对内核的 API 函数,同步机制的 API 函数等
#include <stdio.h> // 声明 printf 等调试函数
// #include <cyg/io/spce3200_spi.h> // 定义 SPCE3200 的 SPI 模块相关常量,这里含 key

// 定义 SPI 线程的句柄
cyg_io_handle_t hDrvSPI;
void spi_thread(cyg_addrword_t data)
// 通过 SPI 线程进行发送和接收数据
{
    cyg_uint32 len = 1;
    cyg_uint32 RxData[1];
    cyg_uint32 TxData[1];
    TxData[0]=66;
    if (ENOERR != cyg_io_lookup("/dev/spi", &hDrvSPI))
    {
        CYG_TEST_FAIL_FINISH("Error opening /dev/spi");
    }
    printf("Open /dev/spi OK\n");
    while(true)
    {
        len = 1;
        if(ENOERR != cyg_io_write(hDrvSPI, TxData, &len))
            // 调用 cyg_io_write 发送数据
        {
            CYG_TEST_FINISH("Error write /dev/spi");
        }
        printf("SPI TxNum = %d\n",len); // 返回实际发送数据量
        if(len != 0)
		// 发送成功,打印发送数据信息
        {
            printf("SPI TxData = %d\n", TxData[0]);
        }
        if(ENOERR != cyg_io_read(hDrvSPI, RxData, &len))
		// 调用 cyg_io_ read 接收数据
        {
            CYG_TEST_FINISH("Error Read /dev/spi");
        }
        if(len != 0)
		// 接收成功,打印接收数据信息
        {
            printf("SPI RxData = %d\n", RxData[0]);
        }
		// 线程睡眠
        cyg_thread_delay(10);
    }
}

cyg_uint8 stack[4096]; // 线程堆栈
cyg_handle_t simple_threadA; // 线程句柄
cyg_thread thread_s; // 线程信息
void cyg_user_start(void)
{
    CYG_TEST_INIT();
// 创建线程 spi_thread
    cyg_thread_create( 4,
                       spi_thread,
                       (cyg_addrword_t) 0,
                       "can0_thread",
                       (void *) stack, 4096,
                       &simple_threadA,
                       &thread_s);
    cyg_thread_resume(simple_threadA);
    cyg_scheduler_start();
}

