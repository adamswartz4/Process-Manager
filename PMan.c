/*
Adam Swartz
September 25, 2021
*/

#include <sys/types.h> 
#include <readline/readline.h> 
#include <stdlib.h>
#include <signal.h>   
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>
#include <string.h> 

//function declarations
void bgsig(pid_t pid, char* cmd_type);
void pstat(pid_t pid);
void bglist();
void bgstart(pid_t pid);
void bgkill(pid_t pid);
void bgstop(pid_t pid);
void bg_entry(char** input, size_t input_length);
void check_zombieProcess(void);


//linked list struct
struct node {
	pid_t pid;
	char* process;
	struct node * next;
};

//prototype for linked list methods
void push(pid_t new_pid, char* new_process);
void print_list(struct node* curr);
int checkpid(struct node* curr, pid_t check);
void removeprocess(pid_t key);

struct node* head = NULL;

void waitfunction(){ //this function checks the current state of processes and outputs new information
int status; pid_t pid;
while(1){
pid= waitpid(-1, &status, WCONTINUED | WNOHANG | WUNTRACED); //check pid in queue	
if(pid>0){ //if the pid exists
	if(WIFSTOPPED(status)){ //if the status is stopped
		printf("Process %d stopped\n", pid);
	}
	if(WIFEXITED(status)){ //if the status is terminated
		removeprocess(pid);
		printf("Process %d terminated\n", pid);
	}
	if(WIFCONTINUED(status)){ //if the status is started
		printf("Process %d started\n", pid);
	}
	if(WIFSIGNALED(status)){ //if the status is killed
		removeprocess(pid);
		printf("Process %d killed\n", pid);
	}
	check_zombieProcess(); //check for processes running in zombie mode
}
else{
	return;
}
}

}

int main(){
	
	while(1){	
		waitfunction(); //check current state of processes 
		char** input= (char**) malloc(20 *sizeof(char*)); //initialize array for input

		char * cmd = readline("PMan: > ");
		char* token = strtok(cmd, " "); // split input with space as deliniator
		int index=0;
		while(token!=NULL){ //store input in array
		input[index++]=token;
		token = strtok(NULL, " ");
		}
		input[index]=NULL;
		size_t input_length=index;

		
		
		if(input[0]==NULL){ //check input

		}
		else if (strcmp(input[0], "bg") == 0){
			if(input[1]==NULL){ //no input error
				printf("ERROR: Please provide process name\n");
			}
			else{
			bg_entry(input, input_length);
			}
		}
		else if(strcmp(input[0], "bglist") == 0){
			bglist();
		}
		else if(strcmp(input[0], "bgkill") == 0 || strcmp(input[0], "bgstop") == 0 || strcmp(input[0], "bgstart") == 0){
			
			if(input[1]==NULL){
				printf("ERROR: Please provide PID\n");
			}
			else{
			pid_t pid = atoi (input[1]); //convert str to pid_t
			bgsig(pid, input[0]);
			}
		}
		else if(strcmp(input[0], "pstat") == 0){
			if(input[1]==NULL){
				printf("ERROR: Please provide PID\n");
			}
			else{
			pid_t pid = atoi (input[1]);
			pstat(pid);
			}
		}
		else {
			printf("command not found\n"); //incorrect command entered
			
		}
		
	}
	
	
	
	
}


void bgsig(pid_t pid, char* cmd_type){ 
	/*
	checks for errors and then begins 
	method based off of user input command signal
	*/
	int check= checkpid(head,pid);
	if(check==0){
		printf("Error: Process %d does not exist\n", pid);
	}
	else{
	
	//printf("bgsig!\n");
		if(strcmp(cmd_type, "bgkill") == 0){
			bgkill(pid);
		}
		else if(strcmp(cmd_type, "bgstop") == 0){
			bgstop(pid);
		}
		else if(strcmp(cmd_type, "bgstart") == 0){
			bgstart(pid);
		}
	}
}


void pstat(pid_t pid){
	int check= checkpid(head,pid); //check if pid exists
	if(check==0){
		printf("Error: Process %d does not exist\n", pid);
	}
	else{
		int i=0;
		char lines[128][128]; //create blank arrays for file output
		char path[128];
		sprintf(path, "/proc/%d/status", pid); //read status path into variable
		FILE *status= fopen(path, "r");
    	while(fgets(lines[i], 128, status)) //read through file
		{
        	lines[i][strlen(lines[i]) - 1] = '\0'; //input file into array
        	i++;
    	}
		i=0;
		char lines2[128][128];
		char path2[128];
		sprintf(path2, "/proc/%d/stat", pid); //read stat path into variable
		FILE *stat= fopen(path2, "r");
    	while(fgets(lines2[i], 128, stat))  //read through file 
		{
        	lines2[i][strlen(lines2[i]) - 1] = '\0'; //input file into array
        	i++;
    	}

	char* c;
	long unsigned int utime = strtoul(lines2[14], &c, 10) / sysconf(_SC_CLK_TCK); //divide utime by sysconf(_SC_CLK_TCK)
	long unsigned int stime = strtoul(lines2[15], &c, 10) / sysconf(_SC_CLK_TCK); //divide stime by sysconf(_SC_CLK_TCK)


//print pstat information
		printf("%s\n", lines[0]);
		printf("%s\n", lines[2]);
		printf("utime: %ld\n", utime);
		printf("stime: %ld\n", stime);
		printf("%s\n", lines[23]);
		printf("%s\n", lines[55]);
		printf("%s\n", lines[56]);

	}
}
	/*	 int tot = i;
	printf("\n The content of the file are : \n");    
    for(i = 0; i < tot; ++i)
    {
        printf(" %s\n", lines2[i]);
    }

		
		int tot = i;
		printf("\n The content of the file are : \n");    
    	for(i = 0; i < tot; ++i)
    	{
       		printf(" %s\n", lines[i]);
    	}
		
	//}
	//printf("pstat!\n");

	*/


void bglist(){ //runs linked list method print_list
	print_list(head);
	//printf("bglist!\n");
	
	
}

void bgkill(pid_t pid){ //sends kill signal from given PID
	int signal=kill(pid, SIGTERM);
	if (!signal){
	sleep(1); //sleep for one second to give time for waitfunction()
	return;
	}
	else{
	printf("Error: Failed to kill process\n");
	}
}
void bgstop(pid_t pid){ //sends stop signal from given PID
	int signal=kill(pid, SIGSTOP);
	if (!signal){
	sleep(1); //sleep for one second to give time for waitfunction()
	return;
	}
	else{
	printf("Error: Failed to stop process\n");
	}
}

void bgstart(pid_t pid){ //sends start signal from given PID
	int signal=kill(pid, SIGCONT);
	if (!signal){
	sleep(1); //sleep for one second to give time for waitfunction()
	return;
	}
	else{
	printf("Error: Failed to start process\n");
	}
}



void bg_entry(char** input, size_t input_length){ //runs input program
	//printf("bg entry! %s\n", input);
	char **a = &input[1];
	size_t length =0;
	char *s;
	//loop to find length of input
	for(int i=1; i<input_length; i++){
			s = *a;
			length += strlen(s)+1;
			a++;
	}
	//create new variable to hold the input
	char str[length+1];
	for(int i =0; i<length+1; i++){
		str[i]= '\0';
	}
	//concatenate command into an array
	a = &input[1];				
	for(int i=1; i<input_length; i++){
		s = *a;
		strcat(str, s);
		strcat(str, " "); //add whitespace
		a++;
	}
	str[length+1]='\0';


						
	//run the background process
	a = &input[1];
	pid_t pid;
	pid = fork();
	if(pid == 0){ //child process

		execvp(a[0], a); //run file
		printf("Error on execvp: %s\n", strerror(errno)); //error if file does not run
		exit(1);
		
	}
	else if(pid > 0) { //parent process
		// store information of the background child process in data structure
		push(pid, input[1]);
		printf("Began background process %d\n", pid);
		sleep(1);
	}
	else {
		printf("Error: Fork failed\n");
		return;
	}
	
}


void check_zombieProcess(void){ //method to monitor zombie processes
	int status;
	int retVal = 0;
	
	while(1) {
		usleep(1000);
		if(head == NULL){
			return ;
		}
		retVal = waitpid(-1, &status, WNOHANG);
		if(retVal > 0) {
			removeprocess(retVal); //remove the background process from data structure
			
		}
		else if(retVal == 0){
			break;
		}
		else{
			perror("waitpid failed");
			exit(1);
		}
	}
	return ;
	
}
	//Linked List Methods
	

	void push(pid_t new_pid, char* new_process){ //adds new element to list
	struct node* new_node;	
	new_node = (struct node*) malloc(sizeof(struct node));
	new_node->pid = new_pid;
	new_node->process = new_process;
	new_node->next= head;
	head=new_node;
	}
	

	void print_list(struct node* curr) { //prints list contents
		int count=0;
		while (curr != NULL) {
		count++;
        printf(" %d: ", curr->pid);
		printf(" %s \n", curr->process);
        curr = curr->next;
    	}
	printf("Total background jobs: %d\n", count);
    }
	
	int checkpid(struct node* curr, pid_t check) { //checks if input exists in list
		while (curr != NULL) {
        if(curr->pid==check){
			return 1;
		}
        curr = curr->next;
    	}
	return 0;
    }

	void removeprocess(pid_t key){ //removes key node from list 

		struct node* temp =head;
		struct node* prev =head;
		if (temp != NULL && temp->pid==key){
			head= head->next;
			free(temp);
			return;
		}
		while(temp!=NULL){
			if(temp->pid==key){
				prev->next=temp->next;
				return;
			}
			else{
				if(temp->next==NULL){
					printf("PID not found\n");
					return;
				}
				prev=temp;
				temp=temp->next;
			}
			
		}
		free(temp);


	}






