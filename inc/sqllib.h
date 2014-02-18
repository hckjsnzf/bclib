
#ifndef _SQLLIB_H_
#define _SQLLIB_H_

#include "sqlstruct.h"

int connect_db(char *ps_name, char *ps_pw);
int disconnect_db(int status);
int getyester_fromdb(char *pdate);
int db_insert_bankbilltrade(struct ST_BANKBILL *pst_trade);
int exec_sqlstr(char *pstr);

#endif

