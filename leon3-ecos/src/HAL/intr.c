#include "intr.h"

/*
 * FUNCTIONS:interrupt testing
 * xiaoyang@2012-5-11
 */
static cyg_uint32 
vtask_isr(cyg_vector_t vector,cyg_addrword_t data){
	int priv_data = (int)data;
	/*interrupt msk*/
	cyg_interrupt_mask(priv_data);
	cyg_interrupt_acknowledge(priv_data);

	/*need call DSR*/
	return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

static void 
vtask_dsr(cyg_vector_t vector, cyg_ucount32 count,cyg_addrword_t data){
	int priv_data = (int)data;
	
	/*release intr mask*/
	cyg_interrupt_unmask(priv_data);
}
/*config and attach interrupt*/
void int_vtask(cyg_addrword_t data)
{
	int priv_data = (int) data;
	cyg_interrupt intr;
	cyg_handle_t handle;

	cyg_interrupt_configure(priv_data,0,0);
	//创建句柄
	cyg_interrupt_create(
			priv_data,
			0,                         // Priority
			(cyg_addrword_t)priv_data, // Data item passed to ISR & DSR
			vtask_isr,                    // ISR
			vtask_dsr,                    // DSR
			&handle,                   // handle to intr obj
			&intr);                   // space for int obj
	//将中断挂接到中断句柄上
	cyg_interrupt_attach(handle);
	//使能中断
	cyg_interrupt_unmask(priv_data);

	while(1)
		;
}
