#ifndef __INCLUDE_PLATFORMHELPER_H
#define __INCLUDE_PLATFORMHELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* strcmpi ------------------------------------------- */
#ifdef linux
#include <strings.h>
#define strcmpi strcasecmp
#endif

#endif
