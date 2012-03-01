/*
 * SPI deriver for ecos on DE2-70
 * xiaoyang 
 * 		2012-2-26 11:03 create it.
 *		2012-3-1  16:44	test ok.
 */
 
#include <pkgconf/hal.h>					/*hal 的宏头文件,由系统自动生成*/
#include <pkgconf/devs_spi_leon3.h>	/* spi 驱动 cdl 文件宏头文件,由系统自动生成*/
#include <cyg/infra/cyg_type.h>			/* 定义类型*/
#include <cyg/hal/hal_intr.h>			/* 定义中断相关宏*/
#include <cyg/hal/drv_api.h>			/* 驱动程序头文件*/
#include <cyg/error/codes.h>			/* 定义错误常量,如 ENOERR*/

#if CYGNUM_DEVS_SPI_LEON3_DEBUG_MODE == 1
#include <cyg/infra/testcase.h>			/* 测试宏定义,如 CYG_TEST_CHECK*/
#include <cyg/infra/diag.h>				/* diag 函数声明*/
#include <stdio.h>						/* 调试函数如 printf 等声明*/
#endif

#include <cyg/io/devtab.h>				/* 设备 I/O 入口表定义及声明*/
#include <cyg/io/spi_leon3.h>			/* 定义 LEON3 的 SPI 模块相关常量,这里含 key*/

//-----------------------------------------------------------------------------------//

#define SPI0_REV	1	/* Reverse data*/

//-----------------------------------------------------------------------------------//
// Functions in this module
static Cyg_ErrNo spi_write (cyg_io_handle_t handle, void *buffer, cyg_uint32 * len);
static Cyg_ErrNo spi_read (cyg_io_handle_t handle, void *buffer, cyg_uint32 * len);
static Cyg_ErrNo spi_set_config (cyg_io_handle_t handle, cyg_uint32 key, const void *buffer, cyg_uint32 * len);
static Cyg_ErrNo spi_get_config (cyg_io_handle_t handle, cyg_uint32 key, const void *buffer, cyg_uint32 * len);
static bool spi_init (struct cyg_devtab_entry *tab);
static Cyg_ErrNo spi_lookup (struct cyg_devtab_entry **tab, struct cyg_devtab_entry *st, const char *name);

/*old interface*/
static void old_spi_init(unsigned long spi_base);
static cyg_uint8 old_spi_xmit8(unsigned long spi_base, cyg_uint8 val);

/*xiaoyang add*/
static cyg_bool _SPIDrv_is_open;
static cyg_uint8 TxFlag,RxFlag;
static cyg_sem_t sem_spi_data;

//-----------------------------------------------------------------------------------//
CHAR_DEVIO_TABLE (
	leon3_spi_handlers,			// SPI 设备 I/O 函数表句柄
	spi_write,					// SPI 设备写函数
	spi_read,					// SPI 设备读函数
	NULL,						//
	spi_get_config,				// SPI 读设备设置状态函数
	spi_set_config				// SPI 设备设置函数
	);

CHAR_DEVTAB_ENTRY (
	leon3_spi_device,			// SPI 设备表入口句柄
	CYGDAT_DEVS_SPI_LEON3_NAME,	// 设备名,在 cdl 文件中进行宏定义
	NULL,						//
	&leon3_spi_handlers,		// SPI 设备 I/O 函数表句柄指针
	spi_init,					// SPI 初始化函数
	spi_lookup,					// SPI 设备查找函数
	NULL);

//-----------------------------------------------------------------------------------//
static cyg_uint32 spi_INTService (cyg_uint32 vector, CYG_ADDRWORD data)
{	
	#if CYGNUM_DEVS_SPI_LEON3_DEBUG_MODE == 1	
	/* check is ISR installed ok*/
	//CYG_TEST_CHECK (ISR_DATA == data, "Bad data passed to ISR");
	/* check IRQ43 INT*/
	CYG_TEST_CHECK (43 == vector, "Bad vector passed to ISR");
	printf ("Debug - spi Int Services\n");
	#endif
	
	/* handling IRQ INT*/
	HAL_INTERRUPT_ACKNOWLEDGE (vector);
	/* if SND*/	
	if (TxFlag == 1)			
	{
		/* if SND_INT*/
		if ( 0)
		{
			
		}
	}
	/* if REV*/
	if (RxFlag == 1)
	{
		/* if RCV_INT*/
		if (0)
		{
			
		}
	}
	/* ISR return */
	return CYG_ISR_HANDLED;
}

static Cyg_ErrNo spi_write (cyg_io_handle_t handle, void *buffer, cyg_uint32 * len)
{
	#if CYGNUM_DEVS_SPI_LEON3_DEBUG_MODE == 1	
	printf ("Debug - spi write data\n");
	#endif
	
	cyg_uint8 *bp = (cyg_uint8 *)buffer;
	int i = 0;
	
	/*wait semaphore*/
	cyg_semaphore_wait(&sem_spi_data);
	
	for(i = 0; i < (*len); i++){
		old_spi_xmit8(SPI0_BASE,bp[i]);
	}
	
	/*release semaphore*/
	cyg_semaphore_post(&sem_spi_data);
	
	return ENOERR;
}

/*
 * handle: 
 * buffer: data
 * len: count byte of data to read
 */
static Cyg_ErrNo spi_read (cyg_io_handle_t handle, void *buffer, cyg_uint32 * len)
{
	#if CYGNUM_DEVS_SPI_LEON3_DEBUG_MODE == 1	
	printf ("Debug - spi read data\n");
	#endif
	
	cyg_uint8 *bp = (cyg_uint8 *)buffer;
	int i = 0;
	
	/*wait semaphore*/
	cyg_semaphore_wait(&sem_spi_data);
	
	for(i = 0; i < (*len); i++){
		bp[i] = old_spi_xmit8(SPI0_BASE,0);
	}
	
	/*release semaphore*/
	cyg_semaphore_post(&sem_spi_data);
	return ENOERR;
}


/*
 * handle: 
 * buffer: data
 * len: count byte of data to write
 */
static Cyg_ErrNo spi_set_config (cyg_io_handle_t handle, cyg_uint32 key, const void *buffer, cyg_uint32 * len)
{
#if CYGNUM_DEVS_SPI_LEON3_DEBUG_MODE == 1
	printf ("Debug - spi Set Config\n");
#endif
	
	return ENOERR;
}

static Cyg_ErrNo spi_get_config (cyg_io_handle_t handle, cyg_uint32 key, const void *buffer, cyg_uint32 * len)
{
	return ENOERR;
}

static bool spi_init (struct cyg_devtab_entry *tab)
{
	#if CYGNUM_DEVS_SPI_LEON3_DEBUG_MODE == 1	
	printf ("Debug - spi init\n");
	#endif
	
	/*old spi init*/
	old_spi_init(SPI0_BASE);
	
	/* init a semaphore*/
	cyg_semaphore_init(&sem_spi_data,0);
	/* 
	* WARNNING:
	*		release semaphore，you should do it for waking up the thread 
	*		who uses the semaphore.
	*/
	cyg_semaphore_post(&sem_spi_data);					
	return true;
}
/*
 *
 */
static Cyg_ErrNo spi_lookup (struct cyg_devtab_entry **tab, struct cyg_devtab_entry *st, const char *name)
{
	#if CYGNUM_DEVS_SPI_LEON3_DEBUG_MODE == 1
		printf ("Debug - spi Lookup\n");
	#endif
	return ENOERR;
}


//-----------------------------------------------------------------------------------//
/*old complete*/
/******************************************************************
 * Initialize the SPI Controller
 *
 *****************************************************************/
void old_spi_init(unsigned long spi_base) {
	REG32(spi_base+SPI_CAP) = 0x01018002;
	/* Configure the capability register */
				/* SPI_CAP_FDEPTH = 0x80 => Support FIFO size = 128 */
				/* SPI_CAP_SSEN = 1 */
				/* SPI_CAP_MAXWLEN = 0x0 => Support 32-bit length */
				/* SPI_CAP_SSSZ = 0x01 */
	
	#if SPI0_REV
	REG32(spi_base+SPI_MOD) |= 0x06740000;
	/* Configure the mode register */
									/* SPI_MOD_FACT = 0*/
									/* SPI_MOD_PM = 4 */
									/* SPI_MOD_LEN = 7 */
									/* SPI_MOD_EN = 0 */
			 	 	 	 	 	 	/* SPI_MOD_MS = 1 */
									/* SPI_MOD_REV = 0 */
									/* SPI_MOD_DIV16 = 0 */
									/* SPI_MOD_CPHA = 0 */
									/* SPI_MOD_CPOL = 0 */
									/* SPI_MOD_REV = 1 */ /* MSB transmitted first */
	// REG32(spi_base+SPI_MOD) |= SPI_MOD_CPOL;
	// REG32(spi_base+SPI_MOD) |= SPI_MOD_CPHA;
	#else //SPI_MOD_REV = 0, LSB transmitted first
	REG32(spi_base+SPI_MOD) |= 0x02740000;
	#endif // REV
	REG32(spi_base+SPI_MSK) = 0x0;	/* Disable all interrupts */
	REG32(spi_base+SPI_MOD) |= SPI_MOD_EN;		/* Activate Core */
}

/********************************************************************
 * SPI TX and RX
 *
 *******************************************************************/
cyg_uint8 old_spi_xmit8(unsigned long spi_base,cyg_uint8 val) {
	cyg_uint32 reg32;
  	/* Wait for room */
	while(!(REG32(spi_base+SPI_EVE)&SPI_EVE_NF));
	
	#if SPI0_REV
	REG32(spi_base+SPI_TX) = val<<24&0xFF000000;
	#else
	REG32(spi_base+SPI_TX) = val;
	#endif // REV
	//while(!(REG32(spi_base+SPI_EVE)&SPI_EVE_NE));		/* Wait for transmitting over */
	//while(!((REG32(spi_base+SPI_EVE)&SPI_EVE_LT)));
	while((REG32(spi_base+SPI_EVE)&SPI_EVE_TIP));
	reg32 = REG32(spi_base+SPI_RX);
	//printf("1st: 0x%x\n",reg32);
	
	#if SPI0_REV
	return (cyg_uint8)((reg32>>16)&0xFF);
	#else
	return (cyg_uint8)((reg32>>8)&0xFF);
	#endif // REV
}

cyg_uint16 old_spi_xmit16(unsigned long spi_base, cyg_uint16 val)
{
	while(!(REG32(spi_base+SPI_EVE)&SPI_EVE_NF));
	#if SPI0_REV
		REG32(spi_base+SPI_TX) = val<<16&0xFFFF0000;
	#else
		REG32(spi_base+SPI_TX) = val;
	#endif // REV
	while((REG32(spi_base+SPI_EVE)&SPI_EVE_TIP));
	cyg_uint32 reg32 = REG32(spi_base+SPI_RX);
	//printf("back32: 0x%8x\n",reg32);
	#if SPI0_REV
		return (cyg_uint16)((reg32>>16)&0xFFFF);
	#else
		return (cyg_uint16)(reg32&0xFFFF);
	#endif // REV
}

/*******************************************************************
 * Reconfigure SPI Mode Register
 *
 ******************************************************************/
cyg_uint8 old_spi_reconfig(unsigned long spi_base,cyg_uint32 mask, cyg_uint32 val)
{
	cyg_uint32 mod;
	if(REG32(spi_base+SPI_EVE)&SPI_EVE_TIP) {
		return -1;
	}
	/* Disable the SPI core */
	REG32(spi_base+SPI_MOD) &= (~SPI_MOD_EN);

	mod = REG32(spi_base+SPI_MOD);
	mod = (mod & ~mask) | val;
	REG32(spi_base+SPI_MOD) = mod;

	REG32(spi_base+SPI_MOD) |= SPI_MOD_EN;
	return 0;
}


//-----------------------------------------------------------------------------------//
//end of file//
//-----------------------------------------------------------------------------------//

