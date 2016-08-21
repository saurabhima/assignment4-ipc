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

#define MAX_STR_LENGTH 200

int count_lines(FILE *ptr) {
    rewind(ptr);
    char ch;
    int count = 0;
    while (1) {
        ch = getc(ptr);
        if (ch == EOF)break;
        if (ch == '\n')count++;
    }
    return (count);
}

struct parameters
{
    char parameter_name[100];
    int timing;
};
struct task
{
    char* task_name;
    int total_instances;
    int instance_id;
    struct parameters *para;
    
};

int main(int argc, char *argv[]) {
    int argno = argc;
    if (argno < 4) {
        printf("\nArguement Error..Please Check\n");
        exit(0);
    }
    char *slaveinfo_file, *jobinfo_file;
    char str[MAX_STR_LENGTH];
    slaveinfo_file = argv[2];
    jobinfo_file = argv[1];
    int job_num = atoi(argv[3]);
    printf("\n Slave File Name-->%s", slaveinfo_file);
    printf("\n Job File Name-->%s", jobinfo_file);
    printf("\n Total Jobs-->%d\n", job_num);
    FILE *fileptr1;
    fileptr1 = fopen(slaveinfo_file, "r");
    if (fileptr1 == NULL) {
        fputs("File does not exist", stderr);
        exit(1);
    }
    int lines = count_lines(fileptr1);
    rewind(fileptr1);
    char ***process_profile;
    process_profile = (char ***) malloc((lines + 1) * sizeof (char**));

    char *check;
    int line_no = 0;
    int matchcount = 0;
   
    while (fgets(str, MAX_STR_LENGTH, fileptr1)) {
        process_profile[line_no] = (char **) malloc(1000 * sizeof (char*));
        check = strtok(str, " ");
        
        //printf("\n%s",check);
        int word_count = 0;
        while (check != NULL) {
            process_profile[line_no][word_count] = (char *) malloc(strlen(check) * sizeof (char*));
            strcpy(process_profile[line_no][word_count],check);
            //printf("**%s ",process_profile[line_no][word_count]);
            word_count++;
            
            check = strtok(NULL, " ");
        }
        //printf("\n");
        process_profile[line_no][word_count]="\0";
        line_no++;
    }

    struct task task_details[lines];
    for (int x=0;x<=lines;x++)
    {
        task_details[x].task_name=process_profile[x][0];
        task_details[x].total_instances=atoi(process_profile[x][1]);
        printf("\n[%d]%s - %d ", x,task_details[x].task_name,task_details[x].total_instances);
        int z=2;
        int para_count=-2;
        while(process_profile[x][z]!="\0")
        {
            
            task_details[x].para = struct parameters;
            printf("\n[%d] %s",para_count,process_profile[x][z]);
            strcpy(task_details[x].para.parameter_name,process_profile[x][z++]);
            printf(" %s",process_profile[x][z]);
            task_details[x].para.timing=atoi(process_profile[x][z++]);
            //printf("\n%s - %d ", task_details[x].para[para_count].parameter_name,task_details[x].para[para_count].timing);
            para++;
            //printf("\n%s",process_profile[x][z++]);
            //printf(" %s",process_profile[x][z++]);
        }
        
    }
    





    fclose(fileptr1);
    free(process_profile);
    return (0);
}
