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
/* Counting Lines in File*/

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
/* Structures to Store Slave and Job Parameters */
struct process_parameters {
    char parameter_name[100];
    int timing;
};

struct slave_profile{
    char* task_name;
    int total_instances;
    int instance_id;
    int total_process_parameters;
    struct process_parameters para[10];

};

struct job
{
    char job_type[100];
    char job_parameter[100]; 
};

struct job_profile
{
    char *jobname;
    int job_index;
    struct job job_data[10];

};



/* Main Function*/

int main(int argc, char *argv[]) {
    system("clear");
    printf("Parent Proccess ID-->%ld",(long)getpid());
    int check_master=0;
    int total_child_process = 0;
    char *current_process;
    
    int lines,lines_job;
    
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
    
    
    FILE *fileptr1,*fileptrjob;
    fileptr1 = fopen(slaveinfo_file, "r");
    if (fileptr1 == NULL) {
        fputs("File does not exist", stderr);
        exit(1);
    }
    lines = count_lines(fileptr1);
    lines++;
    printf("\nNo of LInes in Slave File-->%d", lines);
    rewind(fileptr1);
    printf("\nCurrently in Parent Proccess");
    printf("\n Slave File Name-->%s", slaveinfo_file);
    printf("\n Job File Name-->%s", jobinfo_file);
    printf("\n Total Jobs-->%d\n", job_num);
    struct slave_profile slave_details[lines];    //Define Structure for storing Slave Parameters
    char process_profile[lines][100][100];


    char *check;
    int line_no = 0;
    int matchcount = 0;
    
    while (fgets(str, MAX_STR_LENGTH, fileptr1)) {
        //process_profile[line_no] = (char **) malloc(1000 * sizeof (char*));
        char token[2] = " ";
        char *strtemp = strtok(str, "\n");
        check = strtok(strtemp, token);

        //printf("\n%s",check);

        int word_count = 0;
        while (check != NULL) {
            strcpy(process_profile[line_no][word_count], check);
            word_count++;
            check = strtok(NULL, token);
            if (check == "\n")printf("New LIne");
        }
        //printf("\n");
        strcpy(process_profile[line_no][word_count], "\0");
        line_no++;
    }
    fclose(fileptr1);
   
      
    int child_id = -1;
    int x;
    for ( x= 0; x <lines; x++) {
        slave_details[x].total_process_parameters=0;
        slave_details[x].task_name = process_profile[x][0];
        current_process = process_profile[x][0];
        slave_details[x].total_instances = atoi(process_profile[x][1]);
        
        total_child_process = total_child_process + slave_details[x].total_instances;
        int z = 2;
        int para_count = 0;
        while (strcmp(process_profile[x][z], "\0") != 0) {
            strcpy(slave_details[x].para[para_count].parameter_name, process_profile[x][z++]);
            slave_details[x].para[para_count].timing = atoi(process_profile[x][z++]);
            slave_details[x].total_process_parameters++;
            //printf("%s - %d ", slave_details[x].para[para_count].parameter_name, slave_details[x].para[para_count].timing);
            para_count++;

        }
        
        printf("\n[%d]%s - %d ", x, slave_details[x].task_name, slave_details[x].total_instances);

    }

    fileptrjob = fopen(jobinfo_file, "r");
    if (fileptrjob == NULL) {
        fputs("File does not exist", stderr);
        exit(1);
    }
    lines_job = count_lines(fileptrjob);
    lines_job++;
    rewind(fileptrjob);
    //printf("\nNo of LInes in Job File-->%d", lines_job);
    struct job job_details[lines_job];
    char job_profile[lines_job][100][100];
    line_no = 0;
    while (fgets(str, MAX_STR_LENGTH, fileptrjob)) {
        //process_profile[line_no] = (char **) malloc(1000 * sizeof (char*));
        char token[2] = " ";
        char *strtemp = strtok(str, "\n");
        check = strtok(strtemp, token);

        int word_count = 0;
        while (check != NULL) {
            //printf("\n%s",check);
            strcpy(job_profile[line_no][word_count], check);
            word_count++;
            check = strtok(NULL, token);
            if (check == "\n")printf("New LIne");
        }
        //printf("\n");
        strcpy(job_profile[line_no][word_count], "\0");
        line_no++;
    }
    fclose(fileptrjob);


    
    int current_childs = 0;
    int status;
    pid_t pid;
    pid_t pids[total_child_process];
     int current_proccess_instance;
    for (x = 0; x < lines; x++) {
        current_process = process_profile[x][0];
        current_proccess_instance = slave_details[x].total_instances;
        int p;
        for (p = 0; p < current_proccess_instance; p++) {
            check_master=2;
            if ((pids[p] = fork()) < 0) {
                perror("fork");
                abort();
            } else if (pids[p] == 0) {
                //printf("\nCurrently in Child Proccess-->%s ID-->%d\n", current_process,p);
                
                exit(0);
            }
            else if(pids[p]>0)
            {
                //printf("\nCreated Child with Type %s ID-->%ld Parent ID-->%ld\n", current_process,(long)pids[p],(long)getpid());
                 while (current_childs > 0) {
                pid = wait(&status);
                //printf("\nChild with PID %ld exited with status 0x%x.", (long) pid, status);
                --current_childs; // TODO(pts): Remove pid from the pids array.
             }
            
            }

            current_childs++;
        }
    }

   
    


    

    return (0);
}