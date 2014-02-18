

#ifndef _DEAL_CONF_H_
#define _DEAL_CONF_H_



#define FTPSTRING_L 256
#define FILENAME_L 256
#define DB_L  32
#define FILEF_L 32
#define INSIDE_ID 32


#define FROMFILE 0
#define FROMCONFIG 1
#define FROMDATE 2
#define FROMFILENAME 3
#define FROMFILEDATE 4
#define FROMDEFAULT 9

#define TYPE_CHAR 0
#define TYPE_NUM 1
#define TYPE_SEQ 2

typedef struct st_ftp {
	int status;
	int port;
	char server[FTPSTRING_L];
	char user[FTPSTRING_L];
	char pwd[FTPSTRING_L];
	char path[FTPSTRING_L];
	int path_index[3];
	char local_path[FTPSTRING_L];
} st_ftp;

typedef struct db_col {
	char name[DB_L];
	int type;
	int length;
	int source;
	int need_trim;
	char *fromfile;
	char *filedate;
	char value[64];
} st_db_col, *pst_db_col;

typedef struct st_db {
	char info[DB_L];
	char dbname[DB_L];
	int dbnum;
	int need_action;
	st_db_col *dbcol;
} st_db, *pst_db;

typedef struct st_line_format {
	char name[FILEF_L];
	int length;
	char value[FTPSTRING_L*2];
	int check_num;
	char check_value[INSIDE_ID][FILEF_L];
} st_linecol, *pst_linecol;

typedef struct st_line {
	char info[FILEF_L];
	int num;
	pst_linecol linecol;
} st_line, *pst_line;
	
typedef struct st_ff {
	char mode[FILEF_L];
	char chr[4];
	int num;
	pst_line fline;
} st_ff, *pst_ff;

typedef struct st_conf {
	int status;
	int action;
	char name[FILENAME_L];
	char thefile[FILENAME_L];
	char filedate[FILENAME_L];
	int filedate_index[2];
	st_ftp t_ftp;
	int dbnum;
	pst_db t_db;
	pst_ff t_ff;
} st_conf;

int find_fconf_byindex(char *psconf_name, int index, char *psname);
int read_fconf(char *ps_fname, st_conf *pt_conf);
int pre_deal_line(char *psline);


#endif

