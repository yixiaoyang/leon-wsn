
/* Parses hex or decimal number */
unsigned long str2ul (const char *str, char **endptr, int base)
{
	unsigned long number = 0;
	char *pos = (char *) str;
	char *fail_char = (char *) str;
	while (isspace(*pos)) pos++;	/* skip leading whitespace */
	if ((base == 16) && (*pos == '0')) { /* handle option prefix */
		++pos;
		fail_char = pos;
		if ((*pos == 'x') || (*pos == 'X')) ++pos;
	}

  if (base == 0) {		/* dynamic base */
    base = 10;		/* default is 10 */
    if (*pos == '0') {
      ++pos;
      base -= 2;		/* now base is 8 (or 16) */
      fail_char = pos;
      if ((*pos == 'x') || (*pos == 'X')) {
        base += 8;	/* base is 16 */
        ++pos;
      }
    }
  }

  /* check for illegal base */
  if ( !((base < 2) || (base > 36)) )
    while (1) {
      int digit = 40;
      if ((*pos >= '0') && (*pos <= '9')) {
          digit = (*pos - '0');
      } else if (*pos >= 'a') {
          digit = (*pos - 'a' + 10);
      } else if (*pos >= 'A') {
          digit = (*pos - 'A' + 10);
      } else break;

      if (digit >= base) break;

      fail_char = ++pos;
      number = number * base + digit;
    }

  if (endptr) *endptr = fail_char; {
    return number;
  }
}
