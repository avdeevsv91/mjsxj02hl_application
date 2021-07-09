#ifndef _LOGGER_H_
#define _LOGGER_H_

#define LOGGER_LEVEL_FORCED   -1
#define LOGGER_LEVEL_DISABLED 0
#define LOGGER_LEVEL_ERROR    1
#define LOGGER_LEVEL_WARNING  2
#define LOGGER_LEVEL_INFO     3
#define LOGGER_LEVEL_DEBUG    4

// Add message to log
int logger(const char *module, const char *function, const int level, const char *format, ...);

// Hexdump of memory
int logger_memory(const char *description, const int level, const void *address, const int length, int width);

#endif
