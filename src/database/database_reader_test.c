#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <database/database_reader.h>

/**
 * Testes em relação a leitura das base de dados
 */

#define IRIS_DATABASE "databases/iris/iris.data"
#define IRIS_CLASS_COLUMN 4
#define IMAGE_DATABASE  "databases/image/segmentation.data"
#define IMAGE_CLASS_COLUMN 0
#define LETTER_RECOGNITION_DATABASE  "databases/letter-recognition/letter-recognition.data"
#define LETTER_RECOGNITION_CLASS_COLUMN 0

int main(void)
{
    int TOTAL_DATABASE = 10000;
    int SLEEP_TIME = 7;
    int i;
    int j;

    printf("My process ID : %d\n", getpid());
    printf("My parent's ID: %d\n\n", getppid());
    sleep(SLEEP_TIME);
    for (i = 0; i < 10; i++) {
        Database* database = (Database*) malloc(sizeof(Database) * TOTAL_DATABASE);
        for (j = 0; j < TOTAL_DATABASE; j++) {
            database[j] = DatabaseReader_ReadDatabase(IRIS_DATABASE, IRIS_CLASS_COLUMN);
        }
        printf("VAI LIBERAR A MEMÓRIA EM %d SEGUNDOS\n", SLEEP_TIME);
        sleep(SLEEP_TIME);
        for (j = 0; j < TOTAL_DATABASE; j++) {
            Database_Destroy(database[j]);
        }
        free(database);
    }
    printf("MEMÓRIA LIBERADA\n");
    sleep(SLEEP_TIME * 3);
    return 0;
}
