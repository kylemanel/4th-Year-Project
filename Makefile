CC=gcc
OBJDIR=../obj/pure
DOBJDIR=../obj/debug
LOGFILE=../log/build.log
BINDIR=../bin
GENERAL_OPTS=-Wall
LIB_OPTS=-lpthread
OUT_FILE=$(BINDIR)/telenurse
DOUT_FILE=$(BINDIR)/telenurse_debug
C_FILES=main.c BH3_comm.c c_api.c BH3_shared.c BH3_config.c app_config.c
O_FILES=./main.o ./BH3_comm.o ./c_api.o ./BH3_shared.o ./BH3_config.o ./app_config.o
OUT_O_FILES=$(subst ./,$(OBJDIR)/,$(O_FILES))
DOUT_O_FILES=$(subst ./,$(DOBJDIR)/,$(O_FILES))

D_LOGDIR=/var/log/telenurse
D_CONF_DIR=/etc/telenurse
CONF_DIR=../config_files
APP_CONF=telenurse.conf
APP_LOG=telenurse.log
D_BIN_DIR=/usr/bin

##########################Old compile targets#####################
#telenurse: compile
#	@echo "=========Linking pure=============" >> $(LOGFILE)
#	$(CC) $(GENERAL_OPTS) $(OUT_O_FILES) $(LIB_OPTS) -o $(OUT_FILE) >> $(LOGFILE) 2>&1
#	@echo "=========Linking debug============" >> $(LOGFILE)
#	$(CC) $(GENERAL_OPTS) $(DOUT_O_FILES) $(LIB_OPTS) -o $(DOUT_FILE) >> $(LOGFILE) 2>&1
#	@echo "\n+++++++++++++++++++++++++++++++++++++++" >> $(LOGFILE)
#compile:
#	@echo "+++++++++$(shell date)++++++++++++\n" >> $(LOGFILE)
#	@echo "==========Compiling pure==========" >> $(LOGFILE)
#	$(CC) $(GENERAL_OPTS) -c $(C_FILES) $(LIB_OPTS) >> $(LOGFILE) 2>&1
#	mv *.o $(OBJDIR) >> $(LOGFILE) 2>&1
#	@echo "==========Compiling debug=========" >> $(LOGFILE)
#	$(CC) $(GENERAL_OPTS) -c -g $(C_FILES) $(LIB_OPTS) >> $(LOGFILE) 2>&1
#	mv *.o $(DOBJDIR) >> $(LOGFILE) 2>&1
#
###################################################################

ifeq ($(APPUSER)$(MAKECMDGOALS),install)
$(error APPUSER was not set)
endif

.PHONY: all clean

all: c_message $(OUT_FILE) d_c_message $(DOUT_FILE) end

c_message:
	@echo "+++++++++$(shell date)++++++++++++\n" >> $(LOGFILE)
	@echo "==========Compiling pure==========" >> $(LOGFILE)

d_c_message:
	@echo "==========Compiling debug=========" >> $(LOGFILE)

end:
	@echo "\n++++++++++Build log end++++++++++++++++++\n" >> $(LOGFILE)

$(OBJDIR)/%.o: %.c
	$(CC) $(GENERAL_OPTS) -c $< $(LIB_OPTS) -o $@ >> $(LOGFILE) 2>&1

$(OUT_FILE): $(OUT_O_FILES)
	$(CC) $(GENERAL_OPTS) $^ $(LIB_OPTS) -o $@ >> $(LOGFILE) 2>&1
	@echo "=========Linking pure=============" >> $(LOGFILE)


$(DOBJDIR)/%.o: %.c
	$(CC) $(GENERAL_OPTS) -g -c $< $(LIB_OPTS) -o $@ >> $(LOGFILE) 2>&1

$(DOUT_FILE): $(DOUT_O_FILES)
	$(CC) $(GENERAL_OPTS) $^ $(LIB_OPTS) -o $@ >> $(LOGFILE) 2>&1
	@echo "=========Linking debug============" >> $(LOGFILE)

install:
	mkdir -p $(D_LOGDIR) $(D_CONF_DIR)
	cp $(CONF_DIR)/$(APP_CONF) $(D_CONF_DIR)
	chown -R $(APPUSER):$(APPUSER) $(D_LOGDIR)
	chmod -R 644 $(D_LOGDIR) 
	chown -R $(APPUSER):$(APPUSER) $(D_CONF_DIR)
	chmod 700 $(D_CONF_DIR)
	chmod 600 $(D_CONF_DIR)/*
	cp $(OUT_FILE) $(D_BIN_DIR)
	chmod  500 $(D_BIN_DIR)/telenurse
	chown $(APPUSER):$(APPUSER) $(D_BIN_DIR)/telenurse
	cp $(DOUT_FILE) $(D_BIN_DIR)
	chmod  500 $(D_BIN_DIR)/telenurse_debug
	chown $(APPUSER):$(APPUSER) $(D_BIN_DIR)/telenurse_debug
deinstall:
	rm $(D_BIN_DIR)/telenurse
	rm $(D_BIN_DIR)/telenurse_debug
	@echo Config files and logs have not been removed
purge:
	rm $(D_BIN_DIR)/telenurse
	rm $(D_BIN_DIR)/telenurse_debug
	rm -fr $(D_CONF_DIR)
	rm -fr $(D_LOGDIR)
	@echo Config files, binaries and logs have been removed
clean:
	rm $(OUT_O_FILES) $(DOUT_O_FILES) $(OUT_FILE) $(DOUT_FILE)

