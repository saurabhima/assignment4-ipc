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
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_STR_LENGTH 200
#define SHMSZ     27
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

struct slave_profile {
    char* task_name;
    int total_instances;
    int index;
    int instance_id;
    int total_process_parameters;
    struct process_parameters para[10];

};

struct sub_job {
    char job_type[100];
    char job_parameter[100];
};

struct job_profile {
    char *job_name;
    int job_index;
    int total_sub_jobs;
    struct sub_job job_data[10];
    int current_processed_subjob;
};

struct main_job_queue {
    char *job_name;
    int iden;
    int total_sub_jobs;
    int current_sub_job_status;
    int current_sub_job;
};

struct sub_job_parameters {
    char *master_job_name;
    char *parameter;
    int parent_job_index;
};

struct sub_job_queue {
    char *sub_job_name;
    int index;
    int total_sub_jobs;
    int current_sub_job_status;
    struct sub_job_parameters SUB_JOB_PARAMETERS[100];
};

//*************************** Main Function*********************************

int main(int argc, char *argv[]) {
    system("clear");
    int shmid;
    key_t key;
    char *shm, *s;
    key = 5678;
    
    printf("Parent Proccess ID-->%ld", (long) getpid());
    int check_master = 0;
    int total_child_process = 0;
    char *current_process;

    int lines, lines_job;

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


    FILE *fileptr1, *fileptrjob;
    fileptr1 = fopen(slaveinfo_file, "r");
    if (fileptr1 == NULL) {
        fputs("File does not exist", stderr);
        exit(1);
    }
    lines = count_lines(fileptr1);
    lines++;
    //printf("\nNo of LInes in Slave File-->%d", lines);
    rewind(fileptr1);
  //********************Data of Slave File*****************************  
    struct slave_profile slave_details[lines]; //Define Structure for storing Slave Parameters
    char process_profile[lines][100][100];
    
    char *check;
    int line_no = 0;
    int matchcount = 0;

    while (fgets(str, MAX_STR_LENGTH, fileptr1)) {

        char token[2] = " ";
        char *strtemp = strtok(str, "\n");
        check = strtok(strtemp, token);

        int word_count = 0;
        while (check != NULL) {
            strcpy(process_profile[line_no][word_count], check);
            word_count++;
            check = strtok(NULL, token);
            if (check == "\n")printf("New LIne");
        }
        
        strcpy(process_profile[line_no][word_count], "\0");
        line_no++;
    }
    fclose(fileptr1);


    int child_id = -1;
    int x;
    for (x = 0; x < lines; x++) {
        slave_details[x].total_process_parameters = 0;
        slave_details[x].task_name = process_profile[x][0];
        current_process = process_profile[x][0];
        slave_details[x].total_instances = atoi(process_profile[x][1]);
        slave_details[x].index = x;
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
    printf("\n******************");
    fflush(stdout);
    fileptrjob = fopen(jobinfo_file, "r");
    if (fileptrjob == NULL) {
        fputs("File does not exist", stderr);
        exit(1);
    }
    lines_job = count_lines(fileptrjob);
    lines_job++;
//********************Data of Job File*****************************
    //printf("\nNo of LInes in Job File-->%d", lines_job);
    struct job_profile job_details[lines_job];
    char job_profile_str[lines_job][100][100];
    line_no = 0;
    char stringjob[MAX_STR_LENGTH];


    rewind(fileptrjob);
    while (fgets(stringjob, MAX_STR_LENGTH, fileptrjob)) {

        int word_count = 0;
        char token[2] = " ";
        char *strtempjob = strtok(stringjob, "\n");

        char *checkjob = strtok(strtempjob, token);

        while (checkjob != NULL) {

            strcpy(job_profile_str[line_no][word_count], checkjob);
            // printf("\n[%d][%d]%s", line_no,word_count,job_profile_str[line_no][word_count]);
            word_count++;

            checkjob = strtok(NULL, "[: ]");
            if (checkjob == "\n")printf("New LIne");
        }
        printf("\n**************");
        strcpy(job_profile_str[line_no][word_count], "\0");
        line_no++;
    }

    fclose(fileptrjob);

    for (x = 0; x < lines_job; x++) {
        job_details[x].total_sub_jobs = 0;
        job_details[x].job_name = job_profile_str[x][0];
        job_details[x].job_index = x;
        //current_process = process_profile[x][0];
        //job_details[x].total_instances = atoi(process_profile[x][1]);

        //total_child_process = total_child_process + slave_details[x].total_instances;
        int z = 1;
        int para_count = 0;

        //printf("\n[%d]%s", x, job_details[x].job_name);
        while (strcmp(job_profile_str[x][z], "\0") != 0) {
            strcpy(job_details[x].job_data[para_count].job_type, job_profile_str[x][z++]);
            strcpy(job_details[x].job_data[para_count].job_parameter, job_profile_str[x][z++]);
            job_details[x].total_sub_jobs++;
            //printf("%s - %s ", job_details[x].job_data[para_count].job_type, job_details[x].job_data[para_count].job_parameter);
            para_count++;

        }

        printf("\n");
        //printf("\n[%d]%s - %d ", x, slave_details[x].task_name, slave_details[x].total_instances);

    }


    struct main_job_queue MAIN_JOB_QUEUE[job_num];


    int verify_job = 0;
    for (x = 0; x < job_num; x++) {
        if (verify_job >= lines_job)verify_job = 0;
        MAIN_JOB_QUEUE[x].job_name = job_details[verify_job].job_name;
        MAIN_JOB_QUEUE[x].iden = x;
        MAIN_JOB_QUEUE[x].total_sub_jobs = job_details[verify_job].total_sub_jobs;
        MAIN_JOB_QUEUE[x].current_sub_job_status = 0;
        MAIN_JOB_QUEUE[x].current_sub_job= 0;
        //printf("\n%s [%d]",MAIN_JOB_QUEUE[x].job_name,MAIN_JOB_QUEUE[x].iden);
        verify_job++;
    }
    fflush(stdout);
    //**************************************************Sub Proccess/Slave Queue Creation Module********************************************
    
    //***********************************************Shared Memory Creation*********
    if ((shmid = shmget(key,27 , IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
     if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    s = shm;
    strcpy(s,"Saurabh SIngh");
    //************************************************Proccess Creation Module************************************************************   
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
            check_master = 2;
            if ((pids[p] = fork()) < 0) {
                perror("fork");
                abort();
            } else if (pids[p] == 0) {
                //printf("\nCurrently in Child Proccess-->%s ID-->%d\n", current_process,p);
                printf("\nCurrently sub Proccess-->%s[%d]", current_process, p);
                
               
                int total_instances;
                int instance_id;
                int total_proccess_parameters;
                for (x = 0; x < lines; x++) {

                    if (strcmp(slave_details[x].task_name, current_process) == 0) {
                        total_instances = slave_details[x].total_instances;
                        total_proccess_parameters = slave_details[x].total_process_parameters;
                        break;
                    }
                    instance_id =slave_details[x].instance_id;

                           
                }

                exit(0);
            } else if (pids[p] > 0) {
                while (current_childs > 0) {
                //printf("\nCreated Child with Type %s ID-->%ld Parent ID-->%ld\n", current_process,(long)pids[p],(long)getpid());
                /*while(1)
                {
                       int a;
                       for (a=0;a<job_num;a++)
                       {
                               if(MAIN_JOB_QUEUE[x].current_sub_job_status==0)
                               {
                                 MAIN_JOB_QUEUE[x].current_sub_job  
                               }
                       }
                } */   
                    
                    pid = wait(&status);
                    printf("\nChild with PID %ld exited with status 0x%x.", (long) pid, status);
                    --current_childs; // TODO(pts): Remove pid from the pids array.
                }

            }

            current_childs++;
        }
    }


    return (0);
}

int sub_proccess_run(char* name, int time) {
    printf("\nCurrently Proccessing -->%s", name);
    sleep(time);
    return (0);
}