
#ifndef _ALL_H_
#define _ALL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <assert.h>

#include <sys/types.h>
#include <unistd.h>


#include "log.h"
#include "basiclib.h"
#include "deal_conf.h"
#include "conf_to_db.h"
#include "errcode.h"


#include "cuftp.h"


#define LINE_LENGTH 1024*2
#define ZERO(x) memset((x), 0, sizeof(x))

#endif 
