#include "datatrans.h"

int main()
{
	printf("---------------------------\n");
	DataTrans dt;
	if(dt.uart_init(UART1) < 0){
		printf("error:port error\n");
		return -1;	
	}
	if(dt.uart1_test() < 0){
		printf("error:test error\n");
		return -1;
	}
	
	printf("error:test suc!\n");
	printf("---------------------------\n");
	return 0;
}
