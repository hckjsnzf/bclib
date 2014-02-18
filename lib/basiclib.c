
#include "all.h"

static int mond[13]={0, 31,28,31,30,31,30,31,31,30,31,30,31};

static int get_day_now(char *pdate);
static void set_leap_month(char *pyear);


int g_gen_path(const char *pipath, char *popath)
{
        char shome[256];
            
        if(pipath[0] == '/') {
                strcpy(popath, pipath);
        } else if(pipath[0] == '~') {
                strcpy(popath, getenv("HOME"));
                strcat(popath, pipath+1);
        }   
        return 0;
}

/* date and time */
int g_getdate(char *ps_date, char *ps_time)
{
	time_t nowt;
	struct tm *p_ansc_time;
	char s_date[9];
	char s_time[9];

	memset(s_date, 0, sizeof(s_date));
	memset(s_time, 0, sizeof(s_time));

	nowt = time(NULL);
	/*p_ansc_time= gmtime(&nowt);*/
	p_ansc_time= localtime(&nowt);
	sprintf(s_date, "%04d%02d%02d", (p_ansc_time->tm_year)+1900,
		(p_ansc_time->tm_mon)+1,
		p_ansc_time->tm_mday);
	sprintf(s_time, "%02d%02d%02d",
		p_ansc_time->tm_hour,
		p_ansc_time->tm_min,
		p_ansc_time->tm_sec);
	if(ps_date != NULL) {
		memcpy(ps_date, s_date, 8);
	}
	if(ps_time != NULL) {
		memcpy(ps_time, s_time, 6);
	}


	return 0;
}


int get_predate(char *pdate, char *pres, int n)
{
	int i;
	int year,mon,day;
	int yeard;
	char syear[5],smon[3],sday[3];


	if(strlen(pdate) != 8)
		return -1;

	if(!datef_check(pdate))
		return -1;

	if(n == 0) {
		strcpy(pres, pdate);
		return 0;
	}
	if(n < 0) {
		/* TODO */
		/* get_nextdate(pdate, pres, abs(n)); */
		return 0;
	}

	LOG(FL, "s[%s],d", "Now Date", pdate, n);
	ZERO(syear);ZERO(smon);ZERO(sday);
	memcpy(syear, pdate, 4);
	memcpy(smon, pdate+4, 2);
	memcpy(sday, pdate+6, 2);
	year = atoi(syear);
	mon = atoi(smon);
	day = atoi(sday);
	yeard = get_day_now(pdate);
	if( yeard < n) {
		n -= yeard;
		year--;
		mon=12;
		day=31;
	}

	return 0;
}

static int get_day_now(char *pdate)
{
	char smon[3];
	char sday[3];
	int mon,day,all;

	ZERO(smon);ZERO(sday);
	memcpy(smon, pdate+4, 2);
	memcpy(sday, pdate+6, 2);
	mon=atoi(smon);
	day=atoi(sday);

	int i;
	all = 0;
	for(i=1; i<mon; i++) {
		all +=mond[i];
	}
	all += day;
	return all;
}

int datef_check(char *pdate)
{
	char smon[3];
	char sday[3];
	char syear[5];

	ZERO(smon);ZERO(sday);ZERO(syear);
	memcpy(syear, pdate, 4);
	memcpy(smon, pdate+4, 2);
	memcpy(sday, pdate+6, 2);
	if( (atoi(smon) == 0) || (atoi(smon)>12) )
		return 0;

	set_leap_month(syear);

	if( (atoi(sday) > mond[atoi(smon)]) || (atoi(sday) == 0) )
		return 0;
	else
		return 1;
}

static void set_leap_month(char *pyear)
{
	if(is_leapyear(pyear) )
		mond[2] = 29;
	else
		mond[2] = 28;
}

int is_leapyear(char *pyear)
{
	int year;

	year = atoi(pyear);
	if(year%400 == 0)
		return 1;
	else if(year%100 == 0)
		return 0;
	else if(year%4 == 0)
		return 1;
	else
		return 0;
}




int g_get_prog_name(char *ps_in, char *ps_out)
{
	int leng;
	int i;

	leng = strlen(ps_in);

	for(i=leng-1; i>= 0; i--) {
		if(ps_in[i] == '/')
			break;
	}

	strcpy(ps_out, ps_in+i+1);
	return 0;
}


int g_trim(char *ps)
{
	int leng;
	int i;

	if(ps == NULL)
		return 0;

	leng = strlen(ps);
	if(leng == 0)
		return 0;

	for(i = leng-1; i>= 0; i--) {
		if( (ps[i] != ' ')&&(ps[i] != '\t') )
			break;
		ps[i] = '\0';
		leng--;
	}
	if(i == 0)
		return 0;

	for(i = 0; i<leng; i++) {
		if( (ps[i] != ' ')&&(ps[i] != '\t') )
			break;
	}
	memmove(ps, ps+i, leng-i);
	ps[leng-i]='\0';

	return 0;
}


int is_string(char *ps, char *pcons)
{
	if(strcmp(ps, pcons) == 0)
		return 1;
	else
		return 0;
}

int get_index_value(char *psline, char *psindex, char *psvalue)
{
	int i;

	for(i=0; (i<LINE_LENGTH)&&(psline[i]!='\0'); i++) {
		if(psline[i] == '=')
			break;
	}
	memcpy(psindex, psline, i);
	if( i != strlen(psline) )
		strcpy(psvalue, psline+i+1);

	return 0;
}


#define DAYSEC (3600*24)

int get_offday_now(char *pdate, int off)
{
    time_t nowt;
    time_t newt;

    struct tm *pansctime;
    nowt = time(NULL);
    newt = nowt + off*DAYSEC;
    pansctime = localtime(&newt);
    sprintf(pdate, "%04d%02d%02d", (pansctime->tm_year)+1900,
            (pansctime->tm_mon)+1,
            pansctime->tm_mday);

    return 0;
}


/* return str need be free */
char *remove_thischar(char *thisc, const char *sstr)
{
	int i,j;
	int thisc_len, sstr_len;
	char *sout;

	thisc_len = strlen(thisc);
	sstr_len = strlen(sstr);

	sout = calloc(sstr_len, 1);
	if(sout == NULL) {
		return NULL;
	}
	
	for(i = 0,j = 0; i<sstr_len; ) {
		if(memcmp(sstr+i, thisc, thisc_len) == 0)  {
			i = i+thisc_len;
			continue;
		}

		sout[j++] = sstr[i++];
	}

	return sout;
}




