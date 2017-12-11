#ifndef _DATABASE_HANDLER_
#define _DATABASE_HANDLER_

#include "database_struct.h"

#define MY_LINE_MAX 4000

Database readDatabase(const char* path_database, int class_column);
void setFeaturesLenght(char* line, Database database);
Instance readInstance(char* line, int class_column, int features_length);

#endif /*_DATABASE_HANDLER_*/