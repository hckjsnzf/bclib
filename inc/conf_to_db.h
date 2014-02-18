
#ifndef _CONF_TO_DB_H_
#define _CONF_TO_DB_H_

int action_this_db(pst_db pdb, char *psqlstr);
pst_db set_action_flag(pst_line pfline, int ndb, pst_db pdb);
int read_data_toff(char *psline, char *chr, pst_line pfline);
int get_line_n(char *psline, char *chr, int nline, pst_line pfline);
int set_db_file_ref(st_conf *pfconf);
int get_moden(char *psmode, int *pmode_arr);


#endif
