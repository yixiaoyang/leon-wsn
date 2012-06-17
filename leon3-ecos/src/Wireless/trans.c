#include "includes.h"

int rf_tx(unsigned char* tx_buf,unsigned int tx_len,unsigned short dst_rf_addr)
{
    int rc, i;
    unsigned int count = 0;
    //unsigned short dst_addr = 0x2222;
    
    RF_CHIP_INITIALIZE();
    /* Print network configuration */
    netconfig();
    
    while(count < tx_len)
    {
        rc = mac_send(tx_buf,100, (unsigned char*) & dst_rf_addr, 1);
        count += 100;
        delay(1);
    }
    if(rc!=0){
        dprintf("Sending string failed.\n");
    }
    else{
        dprintf("String sent out.\n");
    }
    return 0;
}
 
 
void rf_rx(unsigned char* rx_buf,unsigned int rx_len)
{
    int len, i;
    unsigned pbuf = rx_buf;
    unsigned int count = 0;
     
    RF_CHIP_INITIALIZE();
    netconfig();
    
    dprintf("Receiving...\n");
    while(count < rx_len)
    {
        len = mac_recv(pbuf);
        pbuf += len;
        count += len;
        if(len!=-1)
        {
        }
    }
    dprintf("Rx done!\n");
}
