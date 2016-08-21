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

struct parameters {
    char parameter_name[100];
    int timing;
};

struct task {
    char* task_name;
    int total_instances;
    int instance_id;
    struct parameters para[10];

};

struct slave_manager {
    char *slave_name;
    pid_t child_id;
};

int main(int argc, char *argv[]) {
    int check_master;
    int total_child_process = 0;
    char *current_process;
    
    int lines;
    
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
    
    //printf("\n Slave File Name-->%s", slaveinfo_file);
    //printf("\n Job File Name-->%s", jobinfo_file);
    //printf("\n Total Jobs-->%d\n", job_num);
    FILE *fileptr1;
    fileptr1 = fopen(slaveinfo_file, "r");
    if (fileptr1 == NULL) {
        fputs("File does not exist", stderr);
        exit(1);
    }
    lines = count_lines(fileptr1);
    lines++;
    //printf("\nNo of LInes in Slave File-->%d", ++lines);
    rewind(fileptr1);
    struct task task_details[lines];
    char process_profile[lines][100][100];
if(check_master!=2)
    {
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
    for (int x = 0; x < lines; x++) {
        task_details[x].task_name = process_profile[x][0];
        current_process = process_profile[x][0];
        task_details[x].total_instances = atoi(process_profile[x][1]);
        //printf("\n[%d]%s - %d ", x, task_details[x].task_name, task_details[x].total_instances);
        total_child_process = total_child_process + task_details[x].total_instances;
        int z = 2;
        int para_count = 0;
        while (strcmp(process_profile[x][z], "\0") != 0) {
            strcpy(task_details[x].para[para_count].parameter_name, process_profile[x][z++]);
            task_details[x].para[para_count].timing = atoi(process_profile[x][z++]);
            //printf("%s - %d ", task_details[x].para[para_count].parameter_name, task_details[x].para[para_count].timing);
            para_count++;

        }

    }
    }
    int current_childs = 0;
    int status;
    pid_t pid;
    pid_t pids[total_child_process];
     int current_proccess_instance;
    for (int x = 0; x < lines; x++) {
        current_process = process_profile[x][0];
        current_proccess_instance = task_details[x].total_instances;
        for (int p = 0; p < current_proccess_instance; p++) {
            check_master=2;
            if ((pids[p] = fork()) < 0) {
                perror("fork");
                abort();
            } else if (pids[p] == 0) {
                printf("\nCurrently in Child Proccess-->%s ID-->%d\n", current_process,p);
                //printf("\nCurrent Sub Proccess ID-->%d\n", p);

                exit(0);
            }
            current_childs++;
        }
    }

    while (current_childs > 0) {
        pid = wait(&status);
        printf("\nChild with PID %ld exited with status 0x%x.", (long) pid, status);
        --current_childs; // TODO(pts): Remove pid from the pids array.
    }


    

    return (0);
}