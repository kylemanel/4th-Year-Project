#ifndef BH3_CONFIG_H
#define BH3_CONFIG_H
#include "BH3_shared.h"
int BH3_prepConfigParse(FILE**,char*);
int BH3_parseConfigFile(FILE*,FILE*,CONFIG_ITEM**);
int BH3_endConfigParse();
int BH3_freeConfigItem(CONFIG_ITEM*);

#endif
