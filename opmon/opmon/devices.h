#ifndef DEVICES_H
#define DEVICES_H
/* Vendor codes */
#define VENDOR_GAISLER		0x01
#define VENDOR_ESA				0x04
#define VENDOR_OPENCORES  0x08
#define VENDOR_HITERS     0xF1
const char* vendorid2str(unsigned short id)
{
	switch(id) {
		case VENDOR_GAISLER:
		return "Gaisler Research        ";
		case VENDOR_ESA:
		return "European Space Agency   ";
		case VENDOR_HITERS:
		return "Harin Institute of Tech ";
		default:
		return "Unknown vendor          ";
	}
}
/* Gaisler Research device id's */
#define GAISLER_LEON2DSU 0x002
#define GAISLER_LEON3		 0x003
#define GAISLER_LEON3DSU 0x004
#define GAISLER_ETHAHB   0x005
#define GAISLER_APBMST   0x006
#define GAISLER_AHBUART  0x007
#define GAISLER_SRCTRL   0x008
#define GAISLER_SDCTRL   0x009
#define GAISLER_SSRCTRL  0x00A
#define GAISLER_APBUART  0x00C
#define GAISLER_IRQMP    0x00D
#define GAISLER_AHBRAM   0x00E
#define GAISLER_AHBDPRAM 0x00F
#define GAISLER_GPTIMER  0x011
#define GAISLER_PCITRG   0x012
#define GAISLER_PCISBRG  0x013
#define GAISLER_PCIFBRG  0x014
#define GAISLER_PCITRACE 0x015
#define GAISLER_PCIDMA   0x016
#define GAISLER_AHBTRACE 0x017
#define GAISLER_ETHDSU   0x018
#define GAISLER_CANAHB   0x019
#define GAISLER_GPIO     0x01A
#define GAISLER_AHBROM   0x01B
#define GAISLER_AHBJTAG  0x01C
#define GAISLER_ETHMAC   0x01D
#define GAISLER_SWNODE   0x01E
#define GAISLER_SPW      0x01F
#define GAILSER_AHB2AHB  0x020
#define GAISLER_USBDC    0x021
#define GAISLER_USB_DCL  0x022
#define GAISLER_DDRMP    0x023
#define GAISLER_ATACTRL  0x024
#define GAISLER_DDRSPA   0x025
#define GAISLER_USBEHC   0x026
#define GAISLER_USBUHC   0x027
#define GAISLER_I2CMST   0x028
#define GAISLER_SPW2     0x029
#define GAISLER_AHBDMA   0x02A
#define GAISLER_NUHOSP3  0x02B
#define GAISLER_CLKGATE  0x02C
#define GAISLER_SPICTRL  0x02D
#define GAISLER_DDR2SP   0x02E
#define GAISLER_SLINK    0x02F
#define GAISLER_GRTM     0x030
#define GAISLER_GRTC     0x031
#define GAISLER_GRPW     0x032
#define GAISLER_GRCTM    0x033
#define GAISLER_GRHCAN   0x034
#define GAISLER_GRFIFO   0x035
#define GAISLER_AHBSTAT  0x052
#define GAISLER_KBD      0X060
#define GAISLER_VGA      0x061
#define GAISLER_SVGA     0x063
#define GAISLER_ETHMAC   0x01D
const char* gaisler_devid2str(unsigned short id)
{
	switch(id) {
		case GAISLER_LEON2DSU:
		return "Leon2 Debug Support Unit       ";
		case GAISLER_LEON3:
		return "Leon3 SPARC V8 Processor       ";
		case GAISLER_LEON3DSU:
		return "Leon3 Debug Support Unit       ";
		case GAISLER_ETHAHB:
		return "OC ethernet AHB interface      ";
		case GAISLER_AHBRAM:
		return "Single-port AHB SRAM module    ";
		case GAISLER_AHBDPRAM:
		return "Dual-port AHB SRAM module      ";
		case GAISLER_APBMST:
		return "AHB/APB Bridge                 ";
		case GAISLER_AHBUART:
		return "AHB Debug UART                 ";
		case GAISLER_SRCTRL:
		return "Simple SRAM Controller         ";
		case GAISLER_SDCTRL:
		return "PC133 SDRAM Controller         ";
		case GAISLER_SSRCTRL:
		return "Synchronous SRAM Controller    ";
		case GAISLER_APBUART:
		return "Generic UART                   ";
		case GAISLER_IRQMP:
		return "Multi-processor Interrupt Ctrl.";
		case GAISLER_GPTIMER:
		return "Modular Timer Unit             ";
		case GAISLER_GPIO:
		return "General Purpose I/O port       ";
		case GAISLER_AHBSTAT:
		return "AHB Status Register            ";
		case GAISLER_AHBJTAG:
		return "JTAG Debug Link                ";
		case GAISLER_I2CMST:
		return "AMBA Wrapper for OC I2C-master ";
		case GAISLER_SPICTRL:
		return "SPI Controller                 ";
		default:
		return "Unknown Device                 ";
	}
	
}
/* European Space Agency device id's */
#define ESA_LEON2			0x002
#define ESA_LEON2APB  0x003
#define ESA_IRQ				0x005
#define ESA_TIMER			0x006
#define ESA_UART			0x007
#define ESA_MCTRL			0x00F
const char* esa_devid2str(unsigned short id)
{
	switch(id) {
		case ESA_LEON2:
		return "Leon2 SPARC V8 Processor       ";
		case ESA_LEON2APB:
		return "Leon2 Peripheral Bus           ";
		case ESA_IRQ:
		return "Leon2 Interrupt Controller     ";
		case  ESA_TIMER:
		return "Leon2 Timer                    ";
		case ESA_UART:
		return "Leon2 UART                     ";
		case ESA_MCTRL:
		return "Leon2 Memory Controller        ";
		default:
		return "Unknown Device                 ";
	}
}
/* Get device description by vendor and device id */
const char* get_devicedescript(unsigned short vendor, unsigned short device)
{
	switch(vendor) {
		case VENDOR_GAISLER:
		return gaisler_devid2str(device);
		case VENDOR_ESA:
		return esa_devid2str(device);
		default:
		break;
	}
	return "";
}
#endif // DEVICES_HISLER_LEON3
