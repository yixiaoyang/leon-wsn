#include "common.h"
#include "uart.h"
#include "spr_defs.h"
#include "int.h"
#include "support.h"
#include "build.h"
#include "ctype.h"
#include "LEDs.h"

#define MAX_COMMANDS  100
extern unsigned long src_addr;
extern void testram (unsigned long start_addr, unsigned long stop_addr, unsigned long testno);

bd_t bd;

int num_commands = 0;

command_struct command[MAX_COMMANDS];

void putc (const char c)
{
  debug ("putc %i, %i = %c\n", bd.bi_console_type, c, c);
  switch (bd.bi_console_type) {
  case CT_NONE:
    break;
  case CT_UART:
    uart_putc (c);
    break;
  case CT_CRT:
#if CRT_ENABLED
    screen_putc (c);
#endif
    break;
  case CT_SIM:
    __printf ("%c", c);
    break;
  }
}

int getc ()
{
  int ch = 0;
  debug ("getc %i\n", bd.bi_console_type);
  switch (bd.bi_console_type) {
#if KBD_ENABLED
  case CT_CRT:
    while ((volatile int)kbd_head == (volatile int)kbd_tail);
    ch = kbd_buf[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBDBUF_SIZE;
    return ch;
#endif
  case CT_UART:
    return uart_getc ();
    break;
  case CT_NONE:  //just to satisfy the compiler
  case CT_SIM:
    break;
  }
  return -1;
}

int testc ()
{
  debug ("testc %i\n", bd.bi_console_type);
  switch (bd.bi_console_type) {
#if KBD_ENABLED
  case CT_CRT:
    if (kbd_head == kbd_tail) return 0;
    else return getc ();
#endif
  case CT_UART:
    return uart_testc ();
    break;
  case CT_NONE: // just to satisfy the compiler
  case CT_SIM:
    break;
  }
  return -1;
}

int ctrlc ()
{
  if (testc ()) {
    switch (getc ()) {
      case 0x03:    // ^C - Control C
        return 1;
      default:
        break;
    }
  }
  return 0;
}

unsigned long parse_ip (char *ip)
{
  unsigned long num;
  num = strtoul (ip, &ip, 10) & 0xff;
  if (*ip++ != '.') return 0;
  num = (num << 8) | (strtoul (ip, &ip, 10) & 0xff);
  if (*ip++ != '.') return 0;
  num = (num << 8) | (strtoul (ip, &ip, 10) & 0xff);
  if (*ip++ != '.') return 0;
  num = (num << 8) | (strtoul (ip, &ip, 10) & 0xff);
  return num;
}

void change_console_type (enum bi_console_type_t con_type)
{
  debug ("Console change %i -> %i\n", bd.bi_console_type, con_type);
   //Close previous
  switch (bd.bi_console_type) {
  case CT_NONE:
  case CT_UART:
  case CT_CRT:
  case CT_SIM:
    break;
  }
  bd.bi_console_type = con_type;
   //Initialize new
  switch (bd.bi_console_type) {
  case CT_NONE:
    break;
  case CT_UART:
    uart_init ();
    break;
  case CT_CRT:
#if CRT_ENABLED
    screen_init ();
#endif
#if KBD_ENABLED
    kbd_init ();
#endif
    break;
  case CT_SIM:
    break;
  }
}

void register_command_func (const char *name, const char *params, const char *help, int (*func)(int argc, char *argv[]))
{
  debug ("register_command '%s'\n", name);
  if (num_commands < MAX_COMMANDS) {
    command[num_commands].name = name;
    command[num_commands].params = params;
    command[num_commands].help = help;
    command[num_commands].func = func;
    num_commands++;
  } else printf ("Command '%s' ignored; MAX_COMMANDS limit reached. MAX_COMMANDS = %d, num_commands = %d\n", name, MAX_COMMANDS, num_commands);
}

/* Process command and arguments by executing
   specific function. */
void mon_command(void)
{
  char c = '\0';
  char str[1000];
  char *pstr = str;
  char *command_str;
  char *argv[20];
  u8 argc = 0, chrCnt=0;

   //Show prompt
#ifdef XESS
  printf ("\norp-xsv> ");
#else
  printf ("\n" BOARD_DEF_NAME"> ");
#endif

   //Get characters from UART
  c = getc();

  while (c != '\r' && c != '\f' && c != '\n')
  {
    if (c == '\b') {
    	if(chrCnt>0) {
    		putc('\b');
    		putc(' ');
    		putc('\b');
    		pstr--;
    		chrCnt--;

    	} else {

    	}
    }

    else {
    	chrCnt++;
    	*pstr++ = c;
    	putc(c);
    }

    c = getc();
  }
  *pstr = '\0';
  printf ("\n");

   //Skip leading blanks
  pstr = str;
  while (*pstr == ' ' && *pstr != '\0') pstr++;

  // Get command from the string
  command_str = pstr;

  while (1) {
     //Go to next argument
    while (*pstr != ' ' && *pstr != '\0') pstr++;
		if (*pstr) {
		  *pstr++ = '\0';
		  while (*pstr == ' ') pstr++;
		  argv[argc++] = pstr;
		}
		else
		  break;
  }

  {
    int i, found = 0;

    for (i = 0; i < num_commands; i++)
      if (strcmp (command_str, command[i].name) == 0)
      {
        switch ( command[i].func(argc, &argv[0]) )
        {
          case -1:
            printf ("Missing/wrong parameters, usage: %s %s\n", command[i].name, command[i].params);
            break;
        }

        found++;
        break;
      }
     //'built-in' build command
    if(strcmp(command_str, "build") == 0) {
      printf("Build tag: %s", BUILD_VERSION);
      found++;
    }
    if (!found)
      printf ("Unknown command. Type 'help' for help.\n");
  }
  
}

#if HELP_ENABLED
/* Displays help screen */
int help_cmd (int argc, char *argv[])
{
  int i;
  for (i = 0; i < num_commands; i++) {
	printf("  +-----+-----------------+------------------------------------------+------------------------\n");
	printf("  | %3d | %-15s | %-40s | %s\n", i+1,command[i].name, command[i].params, command[i].help);

  }
	printf("  +-----+-----------------+------------------------------------------+------------------------\n");
  return 0;
}
#endif /* HELP_ENABLED */

void module_cpu_init (void);
void module_memory_init (void);
void model_nic_init(void);
//void module_dhry_init (void);
//void module_camera_init (void);
void module_load_init (void);
void module_mac_init(void);
void module_audio_init(void);
void module_uart_zigbee_init(void);
void tick_init(void);
void module_rf_init(void);
//void module_touch_init (void);
//void module_ata_init (void);
//void module_hdbug_init (void);


/* List of all initializations */
void mon_init (void)
{
   //Set defaults

  //global.erase_method = 2;

  //global.src_addr = (unsigned long)&src_addr;
  //global.dst_addr = FLASH_BASE_ADDR;
	//printf("\nbefore get scr_addr\n");
//uart_getc();
  global.src_addr = (unsigned long)&src_addr;			// Don't know what to do with the address!!!!!!!!!
  //printf("src_addr = 0x%lx\n", global.src_addr);

  global.eth_add[0] = ETH_MACADDR0;
  global.eth_add[1] = ETH_MACADDR1;
  global.eth_add[2] = ETH_MACADDR2;
  global.eth_add[3] = ETH_MACADDR3;
  global.eth_add[4] = ETH_MACADDR4;
  global.eth_add[5] = ETH_MACADDR5;
  global.ip = BOARD_DEF_IP;
  global.gw_ip = BOARD_DEF_GW;
  global.mask = BOARD_DEF_MASK;
  global.srv_ip = (unsigned long)BOARD_DEF_SRV_IP;
  /*  Init modules */
  //module_cpu_init ();
  module_memory_init ();
  model_nic_init();
  module_mac_init();
  //module_eth_init ();
  //module_dhry_init ();
 // module_camera_init ();
  module_audio_init();
  module_load_init ();
  //printf("before zigbee init\n");
  module_uart_zigbee_init();
  module_rf_init();
 // printf("after zigbee init\n");
  //module_touch_init ();
  //module_ata_init ();
  //module_hdbug_init ();

  tick_init();
}

void external_exeption()
{
  //register unsigned char i;
  //register unsigned long PicSr,sr;

}

/* Main shell loop */
int main(int argc, char **argv)
{
	num_commands = 0;
#if 0
  extern unsigned long calc_mycrc32 (void);
  extern unsigned long mycrc32, mysize;
#endif
	// 闪亮LEDs
	open_leds();
	uint8_t i=0;
	for(;i<8;i++) {
	  on_led(i);
	}
  

  int_init ();
  bd.bi_console_type = CT_UART;
  change_console_type (CONSOLE_TYPE);
  mtspr(SPR_SR, mfspr(SPR_SR) | SPR_SR_IEE);

#if SELF_CHECK
  unsigned long t;
  printf ("Self check... ");
  if ((t = calc_mycrc32 ()))
      printf ("FAILED!!!\n");
  else
      printf ("OK\n");
#endif /* SELF_CHECK */

  mon_init ();

/*
  printf("Ramtest 1 start\n");
  testram(VECTOR_END + 0x00004000, 0x00004000, 1);
  printf("Ramtest 1 done\n");

  printf("Ramtest 2 start\n");
  testram(VECTOR_END + 0x0000a000, 0x000a0000, 1);
  printf("Ramtest 2 done\n");
*/

#if HELP_ENABLED
  register_command ("help", "", "shows this help", help_cmd);
#endif

  // Wait user input...
  /*
  uart_getc();
  for(i=0;i<8;i++) {
  	  off_led(i);
  }
	*/

  printf ("\n" BOARD_DEF_NAME " Monitor (type 'help' for help)\n");
  while(1) mon_command();
}
