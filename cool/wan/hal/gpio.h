#ifndef GPIO_H
#define GPIO_H
#include "fpga.h"

#define PORTA			(GPIO_BASE+0x00)		// GPIO[0:7]
#define PORTB			(GPIO_BASE+0x20)		// GPIO[8:15]
#define PORTC			(GPIO_BASE+0x40)		// GPIO[16:23]
#define PORTD			(GPIO_BASE+0x60)		// GPIO[24:31]
#define PORTE			(GPIO_BASE+0x80)		// GPIO[32:39]
#define PORTF			(GPIO_BASE+0xA0)		// GPIO[40:43]

struct gpio_port{
	volatile unsigned long din;
	volatile unsigned long dout;
	volatile unsigned long dir;
	volatile unsigned long imask;
	volatile unsigned long ilevel;
	volatile unsigned long iedge;
	volatile unsigned long aux;
	volatile unsigned long stat;
};

static inline struct gpio_port* gpio_get_port(unsigned long portbase)
{
	return (struct gpio_port*)portbase;
}
/* GPIO function select */
static inline void gpio_select0(struct gpio_port* port, unsigned char mask)
 {
	port->aux &= ~(mask<<8 | mask);
	port->aux |= mask;
}
static inline void gpio_select1(struct gpio_port* port, unsigned char mask)
{
	port->aux &= ~(mask<<8 | mask);
	port->aux |= mask<<8;
}
static inline void gpio_selectno(struct gpio_port* port, unsigned char mask)
{
	port->aux &= ~(mask<<8 | mask);
}

/* set, clear, toggle */
static inline void gpio_make_out(struct gpio_port* port, unsigned char mask)
{
	port->dir &= ~mask;
}
static inline void gpio_set(struct gpio_port* port, unsigned char mask)
{
	port->dout |= mask;
}
static inline void gpio_clr(struct gpio_port* port, unsigned char mask)
{
	port->dout &= ~mask;
}
static inline void gpio_toggle(struct gpio_port* port, unsigned char mask)
{
	port->dout ^= mask;
}

#endif // GPIO_H
