
#include "all.h"


static char dealfilename[FILENAME_L];


int action_this_db(pst_db pdb, char *psqlstr);
pst_db set_action_flag(pst_line pfline, int ndb, pst_db pdb);
int read_data_toff(char *psline, char *chr, pst_line pfline);
int get_line_n(char *psline, char *chr, int nline, pst_line pfline);
int set_db_file_ref(st_conf *pfconf);
int get_moden(char *psmode, int *pmode_arr);

static int check_this_lineformat(char *psline, char *chr, int coln, pst_linecol pcol);
static int bind_db_file(pst_db pdb, pst_line pfl);
static pst_linecol get_linecol_byinfo(char *pname, int ncol , pst_linecol pcolb);
static pst_line check_db_info(char *psdbinfo, int nfline, pst_line pfline);
static int get_strvalue(pst_db_col pdbcol, char *pstr);




int action_this_db(pst_db pdb, char *psqlstr)
{
	int i;
	int ncol;
	
	char sqlstr[LINE_LENGTH*2];
	char svalue[64];

	ncol = pdb->dbnum;
	
	ZERO(sqlstr);
	sprintf(sqlstr, "%s", "insert into");
	sprintf(sqlstr, "%s %s values (", sqlstr, pdb->dbname);
	for(i=0; i<ncol; i++) {
		ZERO(svalue);
		get_strvalue(pdb->dbcol+i, svalue);
		switch( (pdb->dbcol+i)->type) {
		case TYPE_CHAR:
			if( (pdb->dbcol+i)->need_trim == 1) {
				trim(svalue);
			}
			sprintf(sqlstr, "%s '%s'", sqlstr, svalue);
			break;
		case TYPE_NUM:
			sprintf(sqlstr, "%s %d", sqlstr, atoi(svalue));
			break;
		case TYPE_SEQ:
			sprintf(sqlstr, "%s %s.nextval", sqlstr, svalue);
			break;
		default:
			break;
		}

		if(i != (ncol-1) ) {
			strcat(sqlstr, ",");
		}
	}
	sprintf(sqlstr, "%s)", sqlstr);

	LOG(FL, "s", sqlstr);
	strcpy(psqlstr, sqlstr);
	return 0;
}

static int get_strvalue(pst_db_col pdbcol, char *pstr)
{
	char sdate[9];
	char stime[7];

	switch(pdbcol->source) {
	case FROMFILE:
		strcpy(pstr, pdbcol->fromfile);
		break;
	case FROMCONFIG:
		memcpy(pstr, pdbcol->value, pdbcol->length);
		break;
	case FROMDATE:
		ZERO(sdate);ZERO(stime);
		g_getdate(sdate, stime);
		switch (pdbcol->length) {
		case 6:/* yymmdd */
			strcpy(pstr, sdate+2);
			break;
		case 8:/*yyyymmdd*/
			strcpy(pstr, sdate);
			break;
		case 12:/*yymmddhhmmss*/
			strcpy(pstr, sdate+2);
			strcat(pstr, stime);
			break;
		case 14:/*yyyymmddhhmmss*/
			strcpy(pstr, sdate);
			strcat(pstr, stime);
			break;
		default:
			break;
		}
		break;
	case FROMFILENAME:
		strcpy(pstr, dealfilename);
		break;
	case FROMFILEDATE:
		strcpy(pstr, pdbcol->filedate);
		break;
	case FROMDEFAULT:
		strcpy(pstr, pdbcol->value);
		break;
	default:
		break;
	}
	return 0;
}

pst_db set_action_flag(pst_line pfline, int ndb, pst_db pdb)
{
	int i;
	for(i=0; i<ndb; i++) {
		if(is_string(pfline->info, (pdb+i)->info)) {
			(pdb+i)->need_action = 1;
			return pdb+i;
		}
	}
	return NULL;
}

int read_data_toff(char *psline, char *chr, pst_line pfline)
{
	int i,n;
	int inlen, ncp;
	char *s;

	inlen = strlen(psline);
	s = psline;
	n = 0;
	for(i = 0; i<pfline->num; i++) {
		memset( (pfline->linecol+i)->value, 0,
				sizeof( (pfline->linecol+i)->value));
		if( (pfline->linecol+i)->length == 0) {
			ncp=0;
			while(n<inlen) {
				if(psline[n] == chr[0])
					break;
				n++;
				ncp++;
			}
			memcpy( (pfline->linecol+i)->value,
				s, ncp);
			s += ncp+1;
		} else {
			memcpy( (pfline->linecol+i)->value,
				s, (pfline->linecol+i)->length);
			s += (pfline->linecol+i)->length+1;
		}
		if(n >= inlen) {
			LOG(FL, "s", "something wrong");
			return  -1;
		}
	}
	return 0;
}

int get_line_n(char *psline, char *chr, int nline, pst_line pfline)
{
	int i;
	int ret;

	if(nline == 1)
		return 0;

	for(i=0; i<nline; i++) {
		ret = check_this_lineformat(psline, chr, (pfline+i)->num, (pfline+i)->linecol);
		if(ret == 0)
			return i;
	}
	return -1;
}

static int check_this_lineformat(char *psline, char *chr, int coln, pst_linecol pcol)
{
	int i,n;
	char *s;
	int inlen;

	inlen=strlen(psline);
	s = psline;
	n=0;
	for(i=0; i<coln; i++) {
		if( (pcol+i)->length == 0) {
			while(n<inlen) {
				if(*s == chr[0])
					break;
				n++;
				s++;
			}
			if( (i==coln-1) && (*s == '\0') )
				return 0;
			else if( (i==coln-1) && (*s != '\0') )
				return -1;

			if( *s != chr[0] ) 
				return -1;
			n++;s++;
		} else {
			if(strlen(s) < (pcol+i)->length) 
				return -1;

			n += (pcol+i)->length;
			s += (pcol+i)->length;
			if( (i==coln-1) && (*s == '\0') )
				return 0;
			else if( (i== coln-1) && (*s != '\0') )
				return -1;

			if(*s != chr[0])
				return -1;
			n++;s++;
		}
		if(n >= inlen)
			return -1;
	}
	return 0;
}

int set_db_file_ref(st_conf *pfconf)
{
	int ndb;
	char sdbinfo[32];

	int i,ret;
	pst_line fline;

	strcpy(dealfilename, pfconf->name);
	ndb = pfconf->dbnum;
	for(i=0; i<ndb; i++) {
		ZERO(sdbinfo);
		strcpy(sdbinfo, (pfconf->t_db+i)->info);
		fline = check_db_info(sdbinfo, pfconf->t_ff->num, pfconf->t_ff->fline);
		if(fline == NULL) {
			LOG(FL, "s,s,s", "check_db_info error, info", sdbinfo,
				"not included in the file format");
			return -1;
		}
		ret = bind_db_file(pfconf->t_db+i, fline);
		if(ret != 0) {
			LOG(FL, "s","bind_db_file error");
			return ret;
		}
	}
	return 0;
}

static int bind_db_file(pst_db pdb, pst_line pfl)
{
	int i;
	//int ret;
	pst_linecol linecol;
	
	for(i=0; i<pdb->dbnum; i++) {
		if( (pdb->dbcol+i)->source != FROMFILE )
			continue;
		
		linecol=get_linecol_byinfo( (pdb->dbcol+i)->name, pfl->num, pfl->linecol);
		if(linecol == NULL) {
			LOG(FL, "s,s", "get_linecol_byinfo error",
				(pdb->dbcol+i)->name);
			return -1;
		}
		(pdb->dbcol+i)->fromfile = linecol->value;
	}
	return 0;
}

static pst_linecol get_linecol_byinfo(char *pname, int ncol , pst_linecol pcolb)
{
	int i;
	for(i=0; i<ncol; i++) {
		if(is_string(pname, (pcolb+i)->name)) {
			return pcolb+i;
		}
	}
	return NULL;
}

static pst_line check_db_info(char *psdbinfo, int nfline, pst_line pfline)
{
	int i;
	
	for(i=0; i<nfline; i++) {
		if(is_string(psdbinfo, (pfline+i)->info)) {
			return pfline+i;
		}
	}
	return NULL;
}


/* mode_arr not useful now */
int get_moden(char *psmode, int *pmode_arr)
{
	int i,length;
	int nmode;

	nmode=1;
	for(i=0;i<length; i++) {
		if( (psmode[i] == ',') && (psmode[i+1] != ',') 
		    && (i!= 0) && (i!= length-1) )
			nmode++;
	}

	return nmode;
}


