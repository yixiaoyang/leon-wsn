#ifndef TOOLKIT_H
#define TOOLKIT_H

#define SWAP32(x) (((x)&0xFF)<<24 | ((x)&0xFF00)<<8 | (x)>>8&0xFF00 | (x)>>24&0xFF)

unsigned long str2ul (const char *str, char **endptr, int base);
#endif // TOOLKIT_H
