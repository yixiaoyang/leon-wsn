#include <stdio.h>
#include "ambapnp.h"
#include "ahbuart.h"
#include <string.h>
#include <stdlib.h>
#include "devices.h"
/* AMBA Plub&Play Device Configureation Record */
static struct {
    struct amba_device_table ahbmst;
    struct amba_device_table ahbslv;
    struct amba_device_table apbslv;
} amba_conf;
/* AMBA device drivers */
struct amba_driver apb_drivers[] = {
};
struct amba_driver ahb_drivers[] = {
};
unsigned long apbmst=0;
/* insert devices to list */
static int amba_insert_device(struct amba_device_table *tab, unsigned long cfg_addr, int index)
{
	unsigned long cfg_word;
	unsigned char vendor;
	unsigned short device;
	unsigned char version, irq;

	struct amba_device *pamba;
	/* Link list head initialization */
	if(tab->devhead == NULL) {
		tab->devhead = (struct amba_device*)malloc(sizeof(struct amba_device));
		if(tab->devhead==NULL) {
			perror("No more space for amba ahb device\n");
			exit(-1);
		}
		tab->devhead->next = tab->devhead->prev = tab->devhead;
	}
	if(ReadWord(cfg_addr,&cfg_word)) {
		//printf("AHBMST cfg word %#x\n", cfg_word);
		vendor = AMBA_CONF_VID(cfg_word);
		device = AMBA_CONF_DID(cfg_word);
		if(device==0x00) return 0; /* Blank device configuration area */
    /* The details about the device */
		version = AMBA_CONF_VER(cfg_word);
		irq = AMBA_CONF_IRQ(cfg_word);
		/*
		printf(" Vendor ID: %#x\n", vendor);
		printf(" Device ID: %#x\n", device);
		printf(" Version: %#x\n", version);
		printf(" IRQ: %#x\n", irq);
		*/
		pamba = (struct amba_device*)malloc(sizeof(struct amba_device));
		if(pamba==NULL) {
			perror("malloc error\n");
			exit(-1);
		}
		pamba->vendor = vendor;
		pamba->device = device;
		pamba->index = index;
		pamba->addr = cfg_addr;
		/* Memory bank */
		int i;
		unsigned long bar;
		unsigned char type;
		if((cfg_addr&LEON3_IO_AREA)==LEON3_IO_AREA) {
			// AHB membar
			for(i=0;i<4;i++) {
				ReadWord(cfg_addr+(4+i)*4, &bar);
        type = AMBA_MEMBAR_TYPE(bar);
				//printf("Type: %02x\n", type);
				switch(type) {
					case MEMBAR_TYPE_AHBMEM:
						pamba->mem.ahbmem_bar[i].start = MEMBAR_START(bar);
						pamba->mem.ahbmem_bar[i].end = MEMBAR_END(bar);
						if(vendor==VENDOR_GAISLER && device==GAISLER_APBMST) {
							apbmst = pamba->mem.ahbmem_bar[i].start;
						}
						break;
					case MEMBAR_TYPE_AHBIO:
					case MEMBAR_TYPE_APBIO:
					  pamba->mem.ahbmem_bar[i].start = IOBAR_START(LEON3_IO_AREA, bar);
						pamba->mem.ahbmem_bar[i].end = IOBAR_END(LEON3_IO_AREA, bar);
						break;
				}
				pamba->mem.ahbmem_bar[i].type = type;
				//printf("Start: %08x\n", pamba->mem.ahbmem_bar[i].start);
				//printf("Stop: %08x\n", pamba->mem.ahbmem_bar[i].end);
			}
		} else { /* APB IO */
      ReadWord(cfg_addr+4, &bar);
			type = AMBA_MEMBAR_TYPE(bar);
			//printf("Type: %02x\n", type);
			pamba->mem.apbmem_bar.start = IOBAR_START(apbmst, bar);
			pamba->mem.apbmem_bar.end = IOBAR_END(apbmst, bar);
			//printf("Start: %08x\n", pamba->mem.apbmem_bar.start);
		  //printf("Stop: %08x\n", pamba->mem.apbmem_bar.end);
		}
		/* First element */
		if(tab->devhead->prev == tab->devhead) {
			pamba->prev = pamba->next = tab->devhead;
			tab->devhead->prev = tab->devhead->next = pamba;
			tab->devnr++;
		}else {
			// insert new device to list
			pamba->prev = tab->devhead;
      pamba->next = tab->devhead->next;
			tab->devhead->next->prev = pamba;
			tab->devhead->next = pamba;
			tab->devnr++;
		}
	} else {
		return 0;
	}
	return 1;
}
void amba_attach()
{
  /* Attach APB devices */
	int isOnAHB = 0, i=0;
	struct amba_device *pamba = amba_conf.apbslv.devhead->prev, *ptmp;
	while(pamba!=amba_conf.apbslv.devhead) {
		/* Check if there is a AHB correspondance */
		struct amba_device *pahb = amba_conf.ahbmst.devhead->prev;
		while(pahb!=amba_conf.ahbmst.devhead) {
			// AHB Masters 
			if(pamba->vendor==pahb->vendor && pamba->device==pahb->device) {
				isOnAHB = 1;
        break;
			}
      pahb = pahb->prev;
		}
		pahb = amba_conf.ahbslv.devhead->prev;
		while(pahb!=amba_conf.ahbslv.devhead) {
			// AHB Slaves
			if(pamba->vendor==pahb->vendor && pamba->device==pahb->device) {
				isOnAHB = 1;
				break;
			}
      pahb = pahb->prev;
		}
		if(isOnAHB){
		  isOnAHB = 0;
			// Add APB memory configuration to AHB
			for(i=0; i<4; i++) {
				if(pahb->mem.ahbmem_bar[i].type!=MEMBAR_TYPE_AHBMEM &&
					pahb->mem.ahbmem_bar[i].type!=MEMBAR_TYPE_AHBIO) {
					pahb->mem.ahbmem_bar[i].type = MEMBAR_TYPE_APBIO;
					pahb->mem.ahbmem_bar[i].start = pamba->mem.apbmem_bar.start;
					pahb->mem.ahbmem_bar[i].end = pamba->mem.apbmem_bar.end;
				}
			}
			/* Delete this record from apbslv */
			ptmp = pamba->prev;
			pamba->prev->next = pamba->next;
			pamba->next->prev = pamba->prev;
			free(pamba);
			pamba = ptmp;
			continue;
		}
		/* Check APB drivers */
		for(i=0; i<sizeof(apb_drivers)/sizeof(struct amba_driver); i++) {
			if(apb_drivers[i].vendor==pamba->vendor && apb_drivers[i].device==pamba->device) {
				apb_drivers[i].attach(pamba);
				break;
			}
		}
		pamba = pamba->prev;
	}
	/* Attach AHB master devices */
	pamba = amba_conf.ahbmst.devhead->prev;
	while(pamba!=amba_conf.ahbmst.devhead) {
		for(i=0; i<sizeof(ahb_drivers)/sizeof(struct amba_driver); i++) {
			if(ahb_drivers[i].vendor==pamba->vendor && ahb_drivers[i].device==pamba->device) {
				ahb_drivers[i].attach(pamba);
				break;
			}
		}
		pamba=pamba->prev;
	}
	/* Attach AHB slave devices */
	pamba = amba_conf.ahbslv.devhead->prev;
	while(pamba!=amba_conf.ahbslv.devhead) {
		for(i=0; i<sizeof(ahb_drivers)/sizeof(struct amba_driver); i++) {
			if(ahb_drivers[i].vendor==pamba->vendor && ahb_drivers[i].device==pamba->device) {
				ahb_drivers[i].attach(pamba);
				break;
			}
		}
		pamba=pamba->prev;
	}

}
void amba_probe()
{
	unsigned long cfg_addr;
	int i;
	/* Initialize */
	memset(&amba_conf, 0, sizeof(amba_conf));
	/* Probe AHB master */
	cfg_addr = (LEON3_IO_AREA+LEON3_CONF_AREA);
	for(i=0; i<LEON3_AHB_MASTERS;i++) {
		amba_insert_device(&amba_conf.ahbmst, cfg_addr, i);
		cfg_addr+=LEON3_AHB_CONF_WORDS*4;
	}
	/* Probe AHB slaves */
	cfg_addr = (LEON3_IO_AREA+LEON3_CONF_AREA+LEON3_AHB_SLAVE_CONF_AREA);
	for(i=0; i<LEON3_AHB_SLAVES; i++) {
		amba_insert_device(&amba_conf.ahbslv, cfg_addr, i);
		cfg_addr+=LEON3_AHB_CONF_WORDS*4;
	}
	/* Probe APB slaves */
	cfg_addr = apbmst+LEON3_CONF_AREA; //0x800FF000;
	for(i=0; i<LEON3_APB_SLAVES;i++) {
		amba_insert_device(&amba_conf.apbslv, cfg_addr, i);
		cfg_addr+=LEON3_APB_CONF_WORDS*4;
	}
	/* Attach amba devices */
  amba_attach();
	/* Print AMBA Configuration */
	printf("\n");
	printf("Code          %-31s          %-24s\n","Device", "Vendor");
	/* AHB Masters */
	struct amba_device *pamba = amba_conf.ahbmst.devhead->prev;
	while(pamba!=amba_conf.ahbmst.devhead) {
    printf("%02x:%02x.%03x     %s          %s\n",pamba->index, pamba->vendor, pamba->device, \
		get_devicedescript(pamba->vendor, pamba->device),vendorid2str(pamba->vendor));
    pamba = pamba->prev;
	}
	/* AHB Slaves */
	pamba = amba_conf.ahbslv.devhead->prev;
	while(pamba!=amba_conf.ahbslv.devhead) {
    printf("%02x:%02x.%03x     %s          %s\n",pamba->index, pamba->vendor, pamba->device, \
		get_devicedescript(pamba->vendor, pamba->device),vendorid2str(pamba->vendor));
    pamba = pamba->prev;
	}
	/* APB Slaves */
	pamba = amba_conf.apbslv.devhead->prev;
	while(pamba!=amba_conf.apbslv.devhead) {
    printf("%02x:%02x.%03x     %s          %s\n",pamba->index, pamba->vendor, pamba->device, \
		get_devicedescript(pamba->vendor, pamba->device),vendorid2str(pamba->vendor));
    pamba = pamba->prev;
	}

}
