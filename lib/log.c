

#include "all.h"


/* extern inc */




static FILE *fp=NULL;
static char s_fp[256];
static char fdate[9];

static FILE *fconf = NULL;
static char s_fconf[256];

static void set_newfname(char *pfilename, char *pnew_date);



/* log */
int g_init_log(char *ps_filepath, char *ps_fileprev)
{
	char s_filename[256];
	char s_date[9];
	
	memset(s_date, 0, sizeof(s_date));
	memset(s_filename, 0, sizeof(s_filename));
	memset(s_fp, 0, sizeof(s_fp));
	
	g_getdate(s_date, NULL);
	ZERO(fdate);
	strcpy(fdate, s_date);
	sprintf(s_filename, "%s%s_%s.txt", 
		ps_filepath, s_date, ps_fileprev);
	strcpy(s_fp, s_filename);

	fp = fopen(s_filename, "aw");
	if(fp == NULL) {
		printf("error,[%d][%s]\n",
		       errno,strerror(errno));
		return -1;
	}

	return 0;
}


int g_write_log(char *file, int line, char *fmt, ...)
{
	va_list ap;
	int d;
	char c, *s;
	char s_date[11];
	char s_time[9];

	if(fp == NULL) {
		fp = fopen(s_fp, "aw");
	}
	
	memset(s_date, 0, sizeof(s_date));
	memset(s_time, 0, sizeof(s_time));
	g_getdate(s_date, s_time);

	if(! (is_string(s_date, fdate)) ) {
		set_newfname(s_fp, s_date);
		fclose(fp);
		fp=fopen(s_fp, "aw");
	}

	memmove(s_time+3, s_time+2, 4);
	s_time[2]=':';
	memmove(s_time+6, s_time+5, 2);
	s_time[5]=':';
	
	memmove(s_date+5, s_date+4, 4);
	s_date[4]='/';
	memmove(s_date+8, s_date+7, 2);
	s_date[7]='/';

	fprintf(fp, "[%s-%s]-- ", s_date, s_time);
	fprintf(fp, "[%18s][%5d] [%6d]: ", file, line, getpid());
	va_start(ap, fmt);
	while(*fmt)
		switch((c=*fmt++)) {
		case 's':
			s= va_arg(ap, char *);
			fprintf(fp, "%s", s);
			break;
		case 'd':
			d= va_arg(ap, int);
			fprintf(fp, "[%5d]", d);
			break;
		case 'x':
			d= va_arg(ap, int);
			fprintf(fp, "[0X%08x]", d);
			break;
		case 'c':
			c= (char) va_arg(ap, int);
			fprintf(fp, "%c", c);
			break;
		default:
			fprintf(fp, "%c", c);
			break;
		}
	va_end(ap);
	
	fprintf(fp, "\n");
	return 0;
}

int g_write_log_ctl_by_caller(char *file, int line, int filel, int linel, char *fmt, ...)
{
	va_list ap;
	char s_date[11];
	char s_time[9];

	if(fp == NULL) {
		fp = fopen(s_fp, "aw");
	}
	
	memset(s_date, 0, sizeof(s_date));
	memset(s_time, 0, sizeof(s_time));
	g_getdate(s_date, s_time);

	if(! (is_string(s_date, fdate)) ) {
		set_newfname(s_fp, s_date);
		fclose(fp);
		fp=fopen(s_fp, "aw");
	}

	memmove(s_time+3, s_time+2, 4);
	s_time[2]=':';
	memmove(s_time+6, s_time+5, 2);
	s_time[5]=':';
	
	memmove(s_date+5, s_date+4, 4);
	s_date[4]='/';
	memmove(s_date+8, s_date+7, 2);
	s_date[7]='/';

	fprintf(fp, "[%s-%s]-- ", s_date, s_time);
	fprintf(fp, "[%*s][%*d] [%6d]: ", filel, file, linel, line, getpid());
	va_start(ap, fmt);
	vfprintf(fp, fmt, ap);
	va_end(ap);
	
	fprintf(fp, "\n");
	return 0;
}


static void set_newfname(char *pfilename, char *pnew_date)
{
	int i;
	for(i=strlen(pfilename); i>= 0; i--) {
		if(pfilename[i]=='/')
			break;
	}
	memcpy(pfilename+i+1, pnew_date, 8);
	return;
}

void g_close_log(void)
{
	if (fp == NULL)
		return ;
	LOG(FL,"s", "end\n");
	if (fclose(fp) != 0) {
		printf("%d %s\n", errno,strerror(errno));
		g_write_log(__FILE__, __LINE__, "ds", errno, strerror(errno));
	}
}




/* conf */
int g_get_conf(char *ps_filepath, char *ps_fileprev)
{
	char s_filename[256];
	char s_date[9];
	
	memset(s_date, 0, sizeof(s_date));
	memset(s_filename, 0, sizeof(s_filename));
	memset(s_fconf, 0, sizeof(s_fconf));
	
	sprintf(s_filename, "%s%s.conf", 
		ps_filepath, ps_fileprev);
	strcpy(s_fconf, s_filename);
	fconf = fopen(s_filename, "r");
	if(fconf == NULL) {
		printf("fconf error,[%s][%d][%s]\n",
		       s_filename,errno,strerror(errno));
		return -1;
	}

	return 0;
}


void g_close_conf(void)
{
	if(fconf == NULL)
		return ;
	if(fclose(fconf) != 0) {
		g_write_log(FL, "sds", "close fconf",
				errno, strerror(errno));
	}
	return ;
}


int g_read_conf(char *ps_index, char *ps_value)
{
	char s_index[256];
	char s_findex[256];
	char s_fvalue[256];
	char sline[LINE_LENGTH];

	if( (ps_index == NULL) ||
	    (ps_value == NULL) )
		return ERR_ARGU;

	strcpy(s_index, ps_index);
	trim(s_index);
	if(fconf == NULL) {
		fconf = fopen(s_fconf, "r");
	}
	
	assert(fconf);
	fseek(fconf, 0, SEEK_SET);
	while(fgets(sline, 1024, fconf)) {
		if( pre_deal_line(sline) )
			continue;

		ZERO(s_findex); ZERO(s_fvalue);
		get_index_value(sline, s_findex, s_fvalue);
		printf("%s,%s\n", s_findex, s_fvalue);
		trim(s_findex);
		trim(s_fvalue);
		if(strcmp(s_findex, s_index)== 0) {
			strcpy(ps_value, s_fvalue);
			return 0;
		}
	}
	return -1;
}



