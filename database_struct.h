#ifndef _DATABASE_STRUCT_
#define _DATABASE_STRUCT_

#include "array.h"

typedef struct
{
    char* clazz;
    double* features;
    int cluster_id;
    int clazz_code;
} S_Instance;

typedef struct 
{
    char* path_database;
    Array instances;
    int features_length;
} S_Database;

typedef S_Instance* Instance;
typedef S_Database* Database;

Database makeDatabase(const char* path_database, int length);
Instance makeInstance(int features_length);
void printInstance(Instance instance, int features_length);
void printDatabase(Database database);
void freeInstanceGeneric(void* instanceGeneric);
void freeInstance(Instance instance);
void freeDatabase(Database database);

#endif /*_DATABASE_STRUCT_*/