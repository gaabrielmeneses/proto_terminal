#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define ARG_LIM 10
#define INPUT_LIM 500
#define B_SIZE 1000
#define FILENAME_LIM 200


void exec_pwd(void);
void exec_cd(char * dir){
	printf("Diretorio solicitado : %s\n",dir);
	
	if(!chdir(dir)){
		printf("Nao foi possivel acessar: %s\n",dir);
	}

	exec_pwd();
	printf("Deu bom!");
}

void exec_pwd(void){
	char dir[300];
	printf("%s \n",getcwd(dir,300));
}

void exec_ls(void){
	printf("Arquivos e pastas do diretorio atual : \n");
	system("ls");	
}

void exec_piping(char *args1[ARG_LIM], char *args2[ARG_LIM]){
	int pip[2];
	int pstat = pipe(pip);

	if(pstat < 0){
		
		printf("Erroooou ao tentar usar pipe\n");
		exit(1);
	}

	int filho = fork();

	if(filho < 0){
		printf("Erroooou no fork\n");
		exit(1);
	}
	else if(filho == 0){
		close(1);
		close(pip[0]);
		dup(pip[1]);
		execvp(args1[0],args1);
		exit(1);
	}
	else{
		wait(NULL);
		close(0);
		close(pip[1]);
		dup(pip[0]);
		execvp(args2[0],args2);
		//exit(1);
	}
}

void exec_in_out(char * args[ARG_LIM], char *filename,int mode){
	int arq;
	int i = 0;
	while (args[i]){
		printf("%s\n",args[i++]);
	}

    close(STDOUT_FILENO);
    open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    printf("%s", filename);

    printf("%s", filename);
	execvp(args[0],args);    
	
	execvp(args[0],args);
}


void exec_arq(char * arq){
	char * aux;
	char * args[ARG_LIM];
	char * pargs[ARG_LIM];
	char filename[FILENAME_LIM];
	int i = 0,j = 0;
	int piping = 0;
	int in_out = 0;
	aux = strtok(arq," ");	
	while(aux != NULL){
		if(!strcmp(aux,"|")){

			piping = 1;
			for(j = 0; j<i;j++){
				pargs[j] = args[j];
				args[j] = NULL;
			}
			pargs[j] = NULL;
			i = 0;
		}
		else if(!strcmp(aux,">")){
			args[i] = NULL;
			aux = strtok(NULL," ");
			strcpy(filename,aux);
			exec_in_out(args,filename,1);
			i = 0;
		}
		else if(!strcmp(aux,"<")){
			in_out = 1;
			args[i] = NULL;
			aux = strtok(NULL," ");
			strcpy(filename,aux);
		}
		else{
			args[i++] = aux;
		}

		aux = strtok(NULL," ");
	}
	args[i] = NULL;

	if(piping)
		exec_piping(pargs,args);	
	else if(in_out){
			close(0);
			
			int ap = open(filename, O_RDONLY);
			dup(ap);
			if(fork() == 0){
				execvp(args[0], args);
				exit(1);
			}
			else {
				wait(NULL);
			}
		

		}
	else
		execvp(args[0],args);
}

void exec_funcs(char * str){
	char * arg;
	char entrada[INPUT_LIM];

	char buffer[B_SIZE]; 
	strcpy(entrada,str);
	arg = strtok(str," ");

	if(!strcmp(arg,"exit")){
		printf("Bye !\n");
		exit(0);
	}

	int pfilho = fork();

	if(pfilho < 0){
		printf("O falha na criacao de um processo filho via fork! \n");
		return;
	}
	else if(pfilho == 0){

		if(!strcmp(arg,"cd")){
			arg = strtok(NULL,"");
	
			if(chdir(arg)){
				printf("Nao foi possivel acessar: %s\n",arg);
				printf("Ta errando troca de Diretorio, tente denovo ai ou try again kkkk!\n");
			}
		}
		else if(!strcmp(arg,"pwd")){
			exec_pwd();
		} 
		else {
			exec_arq(entrada);
		}
	}
	else{
		wait(NULL);
	} 
}

int main(void){
	char entrada[INPUT_LIM];
	char dir[300];
	printf("==================================================\n");
	printf("BOM DIA, BOA TARDE, BOA NOITE... WELCOME TO THE TERMINAL!\n");
	printf("Criado por Gabriel Meneses e Lino Mota!\n");
	printf("24 de Setembro de 2020, vivendo a Quarentena!\n");	
	printf("==================================================\n");
	printf("\n");

	while(printf("%s $>",getcwd(dir,300))){
		scanf("%[^\n]%*c",entrada);
		exec_funcs(entrada);
	}

	return 0;
}

