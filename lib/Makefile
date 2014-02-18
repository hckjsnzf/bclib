include ../Make.define

Objbasic = basiclib.o log.o deal_conf.o  conf_to_db.o cuftp.o
Objdb = sqllib.o

all:$(OBJSODB) $(OBJSOB) 

$(OBJSOB):$(Objbasic)
	ar -r $(RUNLIB_PATH)/$@ $(Objbasic)
	

#$(CC) $(CSOFLAG) -o $(RUNLIB_PATH)/$@ $(Objbasic)

$(OBJSODB):$(Objdb)
	ar -r $(RUNLIB_PATH)/$@ $(Objdb) 

	
#$(CC) $(CSOFLAG) -o $(RUNLIB_PATH)/$@ $(Objdb) -L$(ORACLE_HOME)/lib -l clntsh

%.o:%.c
	$(CC) $(CFLAG) -c $<  
	
#$(CC) $(CFLAG) $(CSOFLAG) -c $<  

%.c:%.pc
	$(PROC) iname=$< oname=$@

clean:
	rm -f *.o
	rm -f *.lis
	rm -f sqllib.c
	rm -f $(RUNLIB_PATH)/*.so
	rm -f $(RUNLIB_PATH)/*.a

