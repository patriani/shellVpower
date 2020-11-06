#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **trata_entrada(char *entrada);
int confere(char* comando); 
int teste(char* novo);

int main() {
    char **comando; 
    char *entrada; // leitura bruta da entrada 
    pid_t pid_filho;
    int wstatus; // armazenará status do processo filho
    
    
    while(1){
        entrada = readline("shVpower> "); // printa esse texto e lê o que vier depois dele 
        comando = trata_entrada(entrada); // recebe a entrada bruto e retorna array de comandos legível por execvp() 
       
        if(!comando[0]){ // não aponta para nenhum comando[...]
            printf("erro: Nenhum comando inserido\n"); 
            continue; // próxima iteração e pede entrada novamente
        }else{
            char** novo = malloc(8 * sizeof(char *)); // receberá comando por comando para executar
            int i = 0, j = 0; //variaveis contadoras
            int cat,quit; //variavel para retorno de funções e comparação
            while(comando[i] != NULL){
                cat = strcmp(comando[i],";");
                
                if(cat != 0){
                    quit = strcmp(comando[i],"quit");
                    if(quit == 0){ // se o comando lido for de saida paramos a leitura
                        confere(comando[i]);
                    }
                    novo[j] = comando[i];
                    novo[j+1] = NULL; // caso seja o único comando recebido o array pode ser passado para execvp
                    j++;
                }
                if(cat == 0 || comando[i+1] == NULL){
                    quit = confere(*novo); // caso o retorno for 1 o comando já foi executado 
                    if(quit == 0){ // caso não seja cd ou quit
                        pid_filho = fork(); // cria processo para executar comando
                        if (pid_filho == 0) { // pid == 0 quando eh criado corretamente
                            execvp(novo[0],novo);

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
                
                if(!strcmp(comando[i],";")){ // caso o comando armazenado por novo tenha executado
                    j=0;
                }
                i++;
                
            }
            
            novo = NULL;
            entrada = NULL;
        }
    }
    free(comando);
}

char **trata_entrada(char *entrada) {
    char *parsed; // recebe comando isolado
    char **commando; // alocação dinamica que cria um vetor de endereços2
    int  cont = 0;  
    char *separador = " "; // define um char separador de comandos
    //int temp;
    commando = malloc(8 * sizeof(char *));

    parsed = strtok(entrada, separador); // leitura começa na entrada e vai até um " "

    while (parsed != NULL) { // caso o retorno de strtok seja !=NULL (retornou um ponteiro para string terminada em NULL)
        commando[cont] = parsed; // posição cont de commando recebe endereço do elemento apontado por parsed
        cont++;
        
        parsed = strtok(NULL, separador); // demais pesquisas por " " (faz essa separação até que encontre o final do vetor)
    }
    commando[cont] = NULL; // se entrou no while cont != 0, caso contrário o retorno eh NULL
    return commando;
}
int confere(char* comando){
    int conf1 = -1; // inicialização != 0
    int conf2 = -1; // inicialização != 0
    char **novo;

    novo = malloc(8 * sizeof(char *));
    novo = &comando;

    //printf("novo[0] durante: %s",novo[0]);
    //printf("novo[1] durante: %s",novo[1]);

    // conf1 = strcmp(comando, "quit");
    // conf2 = strcmp(comando, "cd");
    // printf("\nquit: %d\n",conf1);
    // printf("cd: %d\n",conf2);
    // printf("temp: %s\n",comando[0]);

    // if(conf1 == 0){
    //     exit(0);
    // }
    // if(conf2 == 0){
    //     //char* temp = &comando[];
    //     printf("temp: %s\n",comando[2]);
    //     int comp = strcmp(&comando[2],"..");
    //     //printf("temp: %s\n",temp);
    //     if(comp == 0){
    //         chdir("..");
    //         return 1;
    //     }
    //     else{
    //         chdir(&comando[2]);
    //         return 1;
    //     }
    // }
return 0;
}
