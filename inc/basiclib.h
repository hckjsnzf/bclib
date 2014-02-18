
#ifndef _BASICLIB_H_
#define _BASICLIB_H_

#define trim g_trim

int g_getdate(char *ps_date, char *ps_time); /* 取当前的日期和时间 */
int g_get_prog_name(char *ps_in, char *ps_out);
int g_gen_path(const char *pipath, char *popath);
int g_trim(char *ps);
int is_string(char *ps, char *pcons);
int get_index_value(char *psline, char *psindex, char *psvalue);

char *remove_thischar(char *thisc, const char *sstr);


int get_offday_now(char *pdate, int off);

int is_leapyear(char *pyear);
int datef_check(char *pdate);
int get_predate(char *pdate, char *pres, int n);



#endif

