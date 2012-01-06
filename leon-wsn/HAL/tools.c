/*
 * tools.c
 *
 *  Created on: 2011-9-22
 *      Author: xiaoyang
 */

#include "tools.h"

/*
 * Author: xiaoyang
 * Created: 2011-9-22
 * Parameters:
 * Description:for system delay,not correctly.
 */
void delayus(int32u_t us) {
	volatile int m_count;
	while (us-- > 0) {
		m_count = 64;
		while (m_count-- > 0)
			;
	}
}
/*
 * big/little endien check
 * return
 * 		0£ºBig_endia
 * 		1£ºLittle_endia
 */
int check_cpu_endien() {
	union un {
		int a;
		char b;
	} c;
	c.a = 1;
	return (c.b == 1);
}

void testing() {
	int i = 0;
	int j = 0;
	int count = 0;

	char data[3][4];
	char* ptr = &(data[0][0]);

	printf("-----------------------------------\n");
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++) {
			data[i][j] = count++;
		}
	}

	for (i = 0; i < 12; i++) {
		printf("%d ", ptr[i]);
	}
	printf("\n-----------------------------------\n");
	return 0;

}
