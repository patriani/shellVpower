#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **trata_entrada(char *entrada); // separa comando de parâmetro e de outros comandos
int confere(char* comando1, char* comando2); // verifica se o comando eh um "cd [...] " ou "quit"
void flush(); // limpa buffer

int main() {
    char **comando; 
    char *entrada; // leitura bruta da entrada 
    pid_t pid_filho;
    int wstatus; // armazenará status do processo filho
    int is_piped = -1;
    int piped = 0; // recebe retorno de pipe()
    int fd[2]; // canais do pipe
    int len = 0; 
    
    while(1){

        entrada = NULL;
        entrada = readline("shVpower> "); // printa esse texto e lê o que vier depois dele 
        
        len = strlen(entrada);
        if(len <= 512){ // 512 eh o número máximo de caracteres aceitos. " " conta.
        comando = trata_entrada(entrada); // recebe a entrada bruto e retorna array de comandos legível por execvp() 
        }else{
            printf("erro: o limite de caracteres foi atingido\n");
            exit(-1);
        }
        
        if(!comando[0]){ // não aponta para nenhum comando
            printf("erro: Nenhum comando inserido\n"); 
            continue; // próxima iteração e pede entrada novamente
        }
        else{
            char** novo = malloc(8 * sizeof(char *)); // receberá comando por comando para executar
            int i = 0, j = 0; //variaveis contadoras
            int cat,quit; //variavel para retorno de funções e comparação

            while(comando[i] != NULL){
                cat = strcmp(comando[i],";"); //0 se há uma concatenação de comandos
                is_piped = strcmp(comando[i],","); //0 se há pipe 
                if(cat != 0 && is_piped != 0){
                    quit = strcmp(comando[i],"quit");
                    if(quit == 0){ // se o comando lido for de saida paramos a leitura
                        confere(comando[i],NULL);
                    }
                    novo[j] = comando[i];
                    novo[j+1] = NULL; // caso seja o único comando recebido o array pode ser passado para execvp
                    j++;
                }
                if(novo[0]==NULL || novo[0] == " " || novo[0] ==""){
                    printf("erro: numero de comandos insuficiente\n");
                    exit(0);
                }
                if(is_piped == 0){
                    if(pipe(fd) < 0){
                        perror("pipe") ;
                        return -1;
                    }
                    
                    if(fork() == 0){
                        dup2(fd[1],STDOUT_FILENO); //garante que a execução do comando será escrita e não passada para o terminal
                        close(fd[0]); // fecha canal para leitura (sempre ou lê ou escreve)
                         
                        execvp(novo[0],novo);
                        perror("Execução do primeiro comando falhou\n");
                    }

                    int temp1 = (i+1); // comando[i] eh ","
                    
                    printf("comando: %s\n",comando[temp1]);
                    
                    if(comando[temp1] == NULL || comando[temp1] == " " || comando[temp1] =="" || comando[temp1 - 1] == comando[temp1]){
                        printf("erro: numero de comandos insuficiente\n");
                        exit(0);
                    }
                    
                    int temp2 = strcmp(comando[i],";");
                    int temp3 = 0;

                    while(comando[temp1] != NULL && temp2!=0){ 
                        novo[temp3] = comando[temp1];
                        temp3 ++;
                        novo[temp3] = NULL; // caso seja o único comando recebido o array pode ser passado para execvp
                        
                        if(comando[temp1 + 1] == NULL){ // a verificação somente pelo while estava dando por algum motivo não identificado
                            break;
                        }
                        temp1 ++;
                        temp2 = strcmp(comando[temp1],";");
                    }
                    i = temp1;
                    
                    
                    if(fork() == 0) {
                        dup2(fd[0], STDIN_FILENO);
                        close(fd[1]);
                        
                        execvp(novo[0],novo);
                        perror("Execução do segundo comando falhou\n");
                    }
                    
                    novo[0] = NULL; // mesmo que tenha algo em novo[1] o NULL em novo[0] serve como um indicador de que houve pipe e não eh para executar novamente
                    close(fd[0]); 
                    close(fd[1]);
                    wait(0);// tempo para que o primeiro processo filho termine
                    wait(0);// tempo para que o segundo processo filho termine
                
                }
                if(cat == 0 || comando[i+1] == NULL){                    
                    if(novo[0] != NULL){
                        quit = confere(novo[0],novo[1]); // caso o retorno for 1 o comando já foi executado 
                        if(quit == 0){ // caso não seja cd ou quit
                        
                            pid_filho = fork(); // cria processo para executar comando
                            if (pid_filho == 0) { // pid == 0 quando eh criado corretamente
                                execvp(novo[0],novo);
                                printf("\nerro: o comando inserido nao eh aceito\n");
                            }else if(pid_filho == -1){ 
                                perror("fork");
                                exit(EXIT_FAILURE);
                            }
                            
                            // WUNTRACED : retorna quando um processo filho foi parado ("stopped")
                            // WCONTINUED : retorna quando um processo filho parado volta a executar ("resumed")
                            else {
                                
                                // suspende a execução do chamador até que o processo filho especificado pelo pid mude de estado (o primeiro e último parâmetro são -1 e 0 por default) 
                                // o ultimo argumento (valor da "opção") pode ser 0 ou constantes
                                int w = waitpid(pid_filho, &wstatus, WUNTRACED | WCONTINUED);
                                if (w == -1){
                                    perror("waitpid");
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                    }
                }
                
                if(!strcmp(comando[i],";")){ // caso o comando armazenado por novo executou
                    j=0;
                }
                i++;
            }
            free(novo);
            entrada = NULL;
        }
        
        
    }
    free(comando);
}

char **trata_entrada(char *entrada) {
    char *parsed; // recebe comando isolado: se temos "ls -l" parsed recebe primeiro "ls" e depois "-l"
    char **commando; 
    int  cont = 0;  
    char *separador = " "; // define um char separador de comandos

    commando = malloc(8 * sizeof(char *));

    parsed = strtok(entrada, separador); // leitura começa na entrada e vai até um " "

    while (parsed != NULL) { // caso o retorno de strtok seja !=NULL (retornou um ponteiro para string terminada em NULL)
        commando[cont] = parsed; // posição cont de commando recebe endereço do elemento apontado por parsed
        cont++;
        
        parsed = strtok(NULL, separador); // demais pesquisas por " " (a próxima chamada de strtok começando de NULL eh recomendação da documentação)
    }
    commando[cont] = NULL; // se entrou no while cont é diferente de 0 e algo foi passado, caso contrário o retorno eh NULL
    return commando;
}
int confere(char* comando1, char* comando2){ // duas entradas porque geralmente o cd vem acompanhado com ".." ou com diretorio
    int conf1 = -1; // inicialização != 0
    int conf2 = -1; 


     conf1 = strcmp(comando1, "quit");
     conf2 = strcmp(comando1, "cd");

    if(conf1 == 0){ // quit
        exit(0);
    }
     if(conf2 == 0){ // cd
         int comp = strcmp(comando2,".."); // cd ..
         if(comp == 0){
             chdir("..");
             return 1;
         }
        else if(comando2 != NULL){ // cd "diretorio"
            chdir(comando2);
            return 1;
        }
        else{ // caso não bata com nenhum diretorio listado e não seja ".."
            printf("\ncd: argumento de CD nao reconhecido");
            return 0;
        }
     }
return 0;
}

void flush() { 
    int ch;
    while ( ( ch = fgetc ( stdin ) ) != EOF && ch != '\n' ) {}
}
