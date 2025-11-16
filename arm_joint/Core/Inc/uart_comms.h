#ifndef __SWO_H
#define __SWO_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"
#include "usart.h"


void log_error(const char *format, ...);
void log_info(const char *format, ...);
void log_debug(const char *format, ...);
void log_warn(const char *format, ...);
//void log_warn(char *p);

#endif
