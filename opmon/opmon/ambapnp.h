#ifndef AMBAPNP_H
#define AMBAPNP_H
/* AMBA Plug&Play Configuration Area */
#define LEON3_IO_AREA			0xFFF00000
#define LEON3_CONF_AREA		0xFF000
#define LEON3_AHB_SLAVE_CONF_AREA	(1<<11)
/* Configuration Record Size and Number */
#define LEON3_AHB_CONF_WORDS 8
#define LEON3_APB_CONF_WORDS 2

#define LEON3_AHB_MASTERS 8
#define LEON3_AHB_SLAVES 8
#define LEON3_APB_SLAVES 16
/* Device identification register fields */
#define AMBA_CONF_VID_MASK 0x000000FF
#define AMBA_CONF_DID_MASK 0x00FFF000
#define AMBA_CONF_VER_MASK 0x000003E0
#define AMBA_CONF_IRQ_MASK 0x0000001F

//#define AMBA_CONF_VID(conf) ((AMBA_CONF_VID_MASK & conf) >> 24)
#define AMBA_CONF_VID(conf) ((conf) & 0xFF)
//#define AMBA_CONF_DID(conf) ((AMBA_CONF_DID_MASK & conf) >> 12)
#define AMBA_CONF_DID(conf) ((((conf) & 0xF00000)>>20)|(((conf) & 0xFF00)>>4))
//#define AMBA_CONF_VER(conf) ((AMBA_CONF_VER_MASK & conf) >> 5)
#define AMBA_CONF_VER(conf) ((((conf)&0x00030000)>>13) | (((conf)&0xE0000000)>>29))
//#define AMBA_CONF_IRQ(conf) (AMBA_CONF_IRQ_MASK & conf)
#define AMBA_CONF_IRQ(conf) ((conf)>>24&0x1F)

struct mem_bar {
	unsigned long start;
	unsigned long end;
#define MEMBAR_TYPE_APBIO			0x01
#define MEMBAR_TYPE_AHBMEM		0x02
#define MEMBAR_TYPE_AHBIO			0x03
	unsigned char type;
};
#define AMBA_MEMBAR_TYPE(bar)  ((bar)>>24&0x0F)
#define IOBAR_START(base, bar) ((((((bar)&0xFF)<<12)|(((bar)&0xF000)>>4))\
 & ((((bar)&0xFF0000)>>4)|((bar)&0xF0000000)>>20)) | (base))
#define IOBAR_END(base, bar) (((((((bar)&0xFF)<<12)|(((bar)&0xF000)>>4))\
 | ((~((((bar)&0xFF0000)>>4)|((bar)&0xF0000000)>>20))&0x000FFFFF)) | (base))+1)
#define MEMBAR_START(bar)  ((((bar)&0xFF0000)<<8)|((bar)&0xF0000000)>>8)\
 & ((((bar)&0xFF)<<24)|((bar)&0xF00)<<12)
#define MEMBAR_END(bar) ((~((((bar)&0xFF0000)<<8)|((bar)&0xF0000000)>>8))\
 | ((((bar)&0xFF)<<24)|((bar)&0xF00)<<12))+1
/* Representation of amba devices */
struct amba_device {
	unsigned long vendor;     // Vendor ID
	unsigned long device;			// Device ID
	unsigned long irq;
	char* name;
	int index;               // Device index on amba
	unsigned long addr;     // Address to configureation record
	struct amba_device* next;
	struct amba_device* prev;
	union amba_mem {
		struct mem_bar ahbmem_bar[4];
		struct mem_bar apbmem_bar;
	} mem;
};
/* AMBA Configuration Table */
struct amba_device_table {
	int devnr; /* number of devices */
	struct amba_device *devhead; /* devices within specific area*/
};	
/* AMBA device drivers */
struct amba_driver {
	unsigned long vendor;
	unsigned long device;
	int (*attach)(struct amba_device* device);
};
/* Function Prototype */
void amba_probe();

#endif // AMBAPNP_H
