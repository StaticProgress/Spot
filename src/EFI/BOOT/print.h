#ifndef PRINT_H
#define PRINT_H

//TODO: Add in formatters when we have variable number of arguments.

/* Returns number of characters printed when successful (excluding null byte).
 * On error returns negative value.
 * https://linux.die.net/man/3/printf
 */
void printf(char *str);

#endif
