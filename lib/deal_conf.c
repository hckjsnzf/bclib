
#include "all.h"


#define CONF_PATH "/home/test/yzw/conf"
#define STACK_L 100

static int stack[STACK_L];
static int * sp = stack;

static int pop(void);
static int push(void);
static int init_stack(void);
static int get_stack(void);

static int deal_ff(st_conf *pt_conf, FILE *fp);
static int deal_fline_format(pst_line pfline, FILE *fp);
static int deal_line_col(pst_linecol plinecol, FILE *fp);

static int deal_db_table(st_conf *pt_conf, FILE *fp);
static int deal_every_table(pst_db pt_db, FILE *fp);
static int deal_table_col(pst_db_col pt_dbcol, FILE *fp);

static int deal_ftp_par(st_conf *pt_conf, FILE *fp );
static int deal_name(st_conf *pt_conf, FILE *fp, char *psn);
static int gen_ftppath(st_conf *pt_conf, char *pstr);


static int pop(void)
{
	if(sp == stack) {
		LOG(FL, "s", "can't pop now, touch the end");
		return -1;
	}
	sp --;
	return *sp;
}
static int push(void)
{
	if(sp == stack+STACK_L) {
		LOG(FL, "s", "can't push now, touch the top");
		return -1;
	}
	sp++;
	return *sp;
}

static int init_stack(void)
{
	int i;
	for(i=0; i<STACK_L; i++) {
		stack[i] = i;
	}
	return 0;
}

static int get_stack(void)
{
	return *sp;
}

int find_fconf_byindex(char *psconf_name, int index, char *psname)
{
	FILE *fp;
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];
	char sinindex[LINE_LENGTH];

	fp = fopen(psconf_name, "r");
	if(fp == NULL) {
		LOG(FL, "s,s, d,s", "find_fconf_byindex error",
			psconf_name,errno, strerror(errno));
		return -1;
	}
	memset(sinindex, 0, sizeof(sinindex));
	sprintf(sinindex, "%d", index);
	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		ZERO(sindex); ZERO(svalue);
		get_index_value(sline, sindex, svalue);
		trim(sindex); trim(svalue);
		if(is_string(sindex, sinindex)) {
			strcpy(psname, svalue);
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return -1;
}

static st_conf *gconf; /* global conf */

int read_fconf(char *ps_fname, st_conf *pt_conf)
{
	char sfilename[FILENAME_L];
	FILE *fp;
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];


	memset(sfilename, 0, sizeof(sfilename));
	strcpy(sfilename, ps_fname);

	fp=fopen(sfilename, "r");
	if(fp == NULL) {
		LOG(FL,"s,s,ds", "fopen error", sfilename, errno, strerror(errno));
		return ERR_FOPEN;
	}
	gconf = pt_conf;

	init_stack();
	while( fgets(sline, sizeof(sline), fp) ) {
		if(pre_deal_line(sline))
			continue;

		memset(sindex, 0, sizeof(sindex));
		memset(svalue, 0, sizeof(svalue));
		get_index_value(sline, sindex, svalue);
		trim(sindex);
		trim(svalue);
		if(strcmp(sindex, "status") == 0) {
			pt_conf->status = atoi(svalue);
		} else if (strcmp(sindex, "action") == 0) {
			pt_conf->action = atoi(svalue);
		} else if (strcmp(sindex, "name") == 0) {
			deal_name(pt_conf, fp, svalue);
		} else if (strcmp(sindex, "ftp") == 0) {
			deal_ftp_par(pt_conf, fp);
		} else if (is_string(sindex, "db")) {
			deal_db_table(pt_conf, fp);
		} else if (is_string(sindex, "ff")) {
			deal_ff(pt_conf, fp);
		}
	}
	fclose(fp);
	return 0;
}

static int deal_ff(st_conf *pt_conf, FILE *fp)
{
	int i;
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];

	pt_conf->t_ff = (pst_ff)malloc(sizeof(st_ff));
	memset(pt_conf->t_ff, 0, sizeof(st_ff));

	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		ZERO(sindex); ZERO(svalue);
		get_index_value(sline, sindex, svalue);
		trim(sindex); trim(svalue);
		if(is_string(sindex, "chr")) {
			char *s;
			s = strtok(svalue, "~");
			strcpy(pt_conf->t_ff->chr, s);
		} else if (is_string(sindex, "mode")) {
			strcpy(pt_conf->t_ff->mode, svalue);
		} else if (is_string(sindex, "num")) {
			int n=atoi(svalue);
			pt_conf->t_ff->num = n;
			pt_conf->t_ff->fline = (pst_line)malloc(n * sizeof(st_line));
			memset(pt_conf->t_ff->fline, 0, n*sizeof(st_line));
			for(i=0; i<n; i++) {
				deal_fline_format(pt_conf->t_ff->fline+i, fp);
			}
		} else if (is_string(sindex, "endff")) {
			return 0;
		}
	}
	return 0;
}

static int deal_fline_format(pst_line pfline, FILE *fp)
{
	int i,n;
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];

	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		ZERO(sindex); ZERO(svalue);
		get_index_value(sline, sindex, svalue);
		trim(sindex); trim(svalue);
		if(is_string(sindex, "info")) {
			strcpy(pfline->info, svalue);
		} else if (is_string(sindex, "num")) {
			n=atoi(svalue);
			pfline->num = n;
			pfline->linecol = (pst_linecol)malloc(n *sizeof(st_linecol));
			memset(pfline->linecol, 0, n*sizeof(st_linecol));
			for(i=0; i<n; i++) {
				deal_line_col(pfline->linecol+i, fp);
			}
		} else if (is_string(sindex, "end")) {
			return 0;
		}
	}

	return 0;
}

static int deal_line_col(pst_linecol plinecol, FILE *fp)
{
	char sline[LINE_LENGTH];
	char sname[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];

	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		break;
	}

	ZERO(sindex); ZERO(svalue);
	get_index_value(sline, sindex, svalue);
	trim(sindex); trim(svalue);

	unsigned int i, name_end;
	for(i = 0; i<strlen(sindex); i++ ) {
		if(sindex[i] == ',')
			break;
	}

	plinecol->length = atoi(svalue);
	if(i == strlen(sindex) ) {
		strcpy(plinecol->name, sindex);
		return 0;
	}
	ZERO(sname);
	memcpy(sname, sindex, i);
	strcpy(plinecol->name, sname);

	int inside_nu=0;
	name_end = i;
	for(++i; i<strlen(sindex); i++) {
		if(sindex[i] == ',') {
			memcpy(plinecol->check_value[inside_nu], sindex+name_end+1, i-(name_end+1));
			plinecol->check_value[inside_nu][i-(name_end+1)] = '\0';
			inside_nu ++;
			name_end = i;
		}
	}
	memcpy(plinecol->check_value[inside_nu], sindex+name_end+1, i-(name_end+1));
	plinecol->check_value[inside_nu][i-(name_end+1)] = '\0';
	plinecol->check_num = inside_nu +1;
	return 0;
}


static int deal_db_table(st_conf *pt_conf, FILE *fp)
{
	int i,n;
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];

	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		break;
	}
	get_index_value(sline, sindex, svalue);/* num */
	trim(sindex); trim(svalue);
	if(!is_string(sindex, "num")) {
		LOG(FL, "s", "deal_db_table error, error num format");
		return -1;
	}
	n=atoi(svalue);
	if(n==0) {
		LOG(FL,"s", "db num is 0!!");
		return -1;
	}
	pt_conf->dbnum = n;
	pt_conf->t_db = (pst_db)malloc(n * sizeof(st_db));
	memset(pt_conf->t_db, 0, n*sizeof(st_db));
	for(i = 0; i <n; i++) {
		deal_every_table(pt_conf->t_db+i, fp);
	}
	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		ZERO(sindex); ZERO(svalue);
		get_index_value(sline, sindex, svalue);
		trim(sindex); trim(svalue);
		if(is_string(sindex, "enddb"))
			return 0;
	}
	return 0;
}

static int deal_every_table(pst_db pt_db, FILE *fp)
{
	int i;
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];

	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		ZERO(sindex); ZERO(svalue);
		get_index_value(sline, sindex, svalue);
		trim(sindex); trim(svalue);

		/* info */
		if(is_string(sindex, "info")) {
			strcpy(pt_db->info, svalue);
		} else if(is_string(sindex,"dbname")) {
			strcpy(pt_db->dbname, svalue);
		} else if(is_string(sindex,"dbnum")) {
			pt_db->dbnum = atoi(svalue);
			pt_db->dbcol = (pst_db_col)malloc(pt_db->dbnum * sizeof(st_db_col));
			memset(pt_db->dbcol, 0, pt_db->dbnum * sizeof(st_db_col));
			for(i=0; i<pt_db->dbnum; i++) {
				deal_table_col(pt_db->dbcol+i, fp);
			}
		} else if (is_string(sindex, "end")) {
			return 0;
		} else
			continue;
	}
	return 0;
}

static int deal_table_col(pst_db_col pt_dbcol, FILE *fp)
{
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];

	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		break;
	}

	ZERO(sindex); ZERO(svalue);
	get_index_value(sline, sindex, svalue);
	trim(sindex); trim(svalue);
	strcpy(pt_dbcol->name, sindex);
	LOG(FL, "s[s]", "deal this db_col", pt_dbcol->name);

	char *s;
	s = strtok(svalue, ","); /* ---type */
	if(is_string(s, "char")) {
		pt_dbcol->type = TYPE_CHAR; /* char */
	} else if(is_string(s, "num")) {
		pt_dbcol->type = TYPE_NUM; /* num */
	} else if(is_string(s, "seq")) {
		pt_dbcol->type = TYPE_SEQ;
	}

	s = strtok(NULL, ","); /* ---length */
	pt_dbcol->length = atoi(s);

	s = strtok(NULL, ","); /* ---source */
	if( memcmp(s, "file", 4) == 0 )
		pt_dbcol->source = FROMFILE; /* file */
	else if( s[0] == '*') {
		pt_dbcol->source = FROMCONFIG; /* default */
		strcpy(pt_dbcol->value, s+1);
	}
	else if( is_string(s, "date") )
		pt_dbcol->source = FROMDATE;
	else if( is_string(s, "thefilename") )
		pt_dbcol->source = FROMFILENAME;
	else if( is_string(s, "thefiledate") ) {
		pt_dbcol->source = FROMFILEDATE;
		pt_dbcol->filedate = gconf->filedate;
		/* strcpy(pt_dbcol->value, gconf->filedate);*/
		/* IMP HERE USING THE GLOBAL GCONF */
	}
	else
		pt_dbcol->source = FROMDEFAULT;

	if(pt_dbcol->type == TYPE_SEQ) {
		strcpy(pt_dbcol->value, s);
	}

	s = strtok(NULL, ","); /* ---trim or not */
	if( is_string(s, "0") ) {
		pt_dbcol->need_trim = 0;
	} else {
		pt_dbcol->need_trim = 1;
	}

	return 0;
}

static int deal_ftp_par(st_conf *pt_conf, FILE *fp )
{
	char sline[LINE_LENGTH];
	char sindex[LINE_LENGTH];
	char svalue[LINE_LENGTH];

	while(fgets(sline, sizeof(sline), fp)) {
		if(pre_deal_line(sline))
			continue;

		memset(sindex, 0, sizeof(sindex));
		memset(svalue, 0, sizeof(svalue));
		get_index_value(sline, sindex, svalue);
		trim(sindex); trim(svalue);
		if( is_string(sindex, "status") ) {
			if(is_string(svalue, "0") ){
				pt_conf->t_ftp.status=0;
				while(fgets(sline, sizeof(sline),fp)) {
					if(pre_deal_line(sline))
						continue;
					if(strcmp(sline, "endftp") == 0)
						return 0;
				}
			}
			pt_conf->t_ftp.status = 1;
		} else if (is_string(sindex, "server")) {
			strcpy(pt_conf->t_ftp.server, svalue);
		} else if (is_string(sindex, "user")) {
			strcpy(pt_conf->t_ftp.user, svalue);
		} else if (is_string(sindex, "pwd")) {
			strcpy(pt_conf->t_ftp.pwd, svalue);
		} else if (is_string(sindex, "path")) {
			/*strcpy(pt_conf->t_ftp.path, svalue);*/
			gen_ftppath(pt_conf, svalue);
		} else if (is_string(sindex, "port")) {
			pt_conf->t_ftp.port= atoi(svalue);
		} else if (is_string(sindex, "local_path")) {
			strcpy(pt_conf->t_ftp.local_path, svalue);
		} else if (is_string(sindex, "endftp")) {
			return 0;
		} else
			continue;
	}
	return 0;
}

static int gen_ftppath(st_conf *pt_conf, char *pstr)
{
	int len,i,begin;
	char smode[16];
	char sbak[512];
	len= strlen(pstr);
	for(i=0; i<len; i++) {
		if(pstr[i] == ',')
			break;
	}
	if(i==len) {
		strcpy(pt_conf->t_ftp.path, pstr);
		return 0;
	}
	pt_conf->t_ftp.path_index[0] = 1;
	begin = i;
	for(i++;i<len;i++) {
		if(pstr[i] == ',')
			break;
	}
	memset(smode, 0, sizeof(smode));
	memcpy(smode, pstr+begin+1, i-begin-1);
	memcpy(pt_conf->t_ftp.path, pstr, begin);
	strcpy(sbak, pstr+i+1);
	pt_conf->t_ftp.path_index[1] = strlen(pt_conf->t_ftp.path);
	if(is_string(smode, "YYYYMMDD")) {
		strcat(pt_conf->t_ftp.path, pt_conf->filedate);
		pt_conf->t_ftp.path_index[2] = 8;
	} else if (is_string(smode, "YYMMDD")) {
		strcat(pt_conf->t_ftp.path, pt_conf->filedate+2);
		pt_conf->t_ftp.path_index[2] = 6;
	} else
		return -1;

	strcat(pt_conf->t_ftp.path, sbak);
	return 0;
}


static int deal_name(st_conf *pt_conf, FILE *fp, char *psn)
{
	int i,j;
	char name[FILENAME_L];
	char sline[LINE_LENGTH];
	char *s;
	char sdate[9];
	char stime[7];
	char sconfig[8];
	char smode[9];
	int namel, datel;

	memset(sdate, 0, sizeof(sdate));
	memset(stime, 0, sizeof(stime));
	memset(name, 0, sizeof(name));
	namel = datel = 0;
	for(i=0; i<atoi(psn); i++) {
		fgets(sline, sizeof(sline), fp);
		for(j=0; (j<LINE_LENGTH)&&(sline[j] != '\0') ;j++) {
			if(sline[j] == '\n') {
				sline[j] = '\0';
				break;
			}
		}
		trim(sline);
		if(sline[0] != ',') {
			strcat(name, sline);
			namel += strlen(sline);
		} else {
			char soff[8];
			int off;
			s = strtok(sline, ",");
			strcpy(sconfig, s);
			s = strtok(NULL, ",");
			strcpy(smode, s);
			s = strtok(NULL, ",");
			strcpy(soff, s);
			off = atoi(soff);
			pt_conf->filedate_index[0] = namel;
			pt_conf->filedate_index[1] = strlen(smode);
			namel += strlen(smode);
			if(strcmp(sconfig, "date") == 0) {
				if(off == 0)
					g_getdate(sdate, NULL);
				else
					get_offday_now(sdate, off);

				strcpy(pt_conf->filedate, sdate);
				if(strcmp(smode, "yymmdd") == 0) {
					memmove(sdate, sdate+2, strlen(sdate+2)+1);
				}
			} else {
			}
			strcat(name, sdate);
		}
	}
	strcpy(pt_conf->name, name);
	LOG(FL, "s: s", "file name", name);
	return 0;
}



int pre_deal_line(char *psline)
{
	int i;

	for(i=0; (i<LINE_LENGTH)&&(psline[i] != '\0'); i++) {
		if(psline[i] == '\n') {
			psline[i]='\0';
			break;
		}
	}
	if( (strlen(psline) == 0) || (psline[0] == '#') )
		return 1;

	i = strlen(psline);
	for( --i; i>=0; i--) {
		if(psline[i] != '\r')
			break;
		psline[i] = '\0';
	}
	/*if(psline[i-1] == '\r')
	  psline[i-1] = '\0';*/

	return 0;
}


