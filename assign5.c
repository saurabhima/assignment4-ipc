#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
int argno = argc;
if (argno < 4) {
        printf("\nArguement Error..Please Check\n");
        exit(0);
    }
char *slaveinfo_file,*jobinfo_file;
slaveinfo_file=argv[1];
jobinfo_file=argv[2];
int job_num=atoi(argv[3]);
printf("\n Slave File Name-->%s",slaveinfo_file);
printf("\n Job File Name-->%s",jobinfo_file);
printf("\n Total Jobs-->%d",job_num);
FILE *fileptr1;
fileptr1 = fopen(slaveinfo_file, "r");
            if (fileptr1 == NULL) {
                fputs("File does not exist", stderr);
                exit(1);
            }
            
char *check;
int matchcount = 0;
    while (fgets(str, MAX_STR_LENGTH, fileptr1)) {
        check = strtok(str, " ");
        while (check != NULL) {
            if (strcmp(check, regexpattern) != 0)
                printf(" %s", check);
            else
                if (strcmp(check, regexpattern) == 0 && flag_value == 0 && notflag != 1)
                printf(" %s", replacement);

            else
                if (strcmp(check, regexpattern) == 0 && flag_value != 0) {
                matchcount++;
                if (matchcount == flag_value) {
                    printf(" %s", replacement);
                }
                else printf(" %s", check);

            }
            check = strtok(NULL, " ");
        }



    }
    

fclose(fileptr1);
return(0);
}