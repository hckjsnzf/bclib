
#ifndef _LOG_H_
#define _LOG_H_

#define LOG g_write_log
#define FL  __FILE__,__LINE__


#define FILEL 20
#define LINEL 3


#define glog(fmt,args...) \
g_write_log_ctl_by_caller(__FILE__, __LINE__, FILEL, LINEL, fmt, ##args)

int g_init_log(char *ps_filepath, char *ps_fileprev); /* 初始化日志文件 */
int g_write_log(char *file, int line, char *fmt, ...); /* 写日志 */
int g_write_log_ctl_by_caller(char *file, int filel, int line, int linel, char *fmt, ...);
void g_close_log(void); /* 关闭日志文件 */


int g_get_conf(char *ps_filepath, char *ps_fileprev);
int g_read_conf(char *ps_index, char *ps_value);
void g_close_conf(void);



#endif

