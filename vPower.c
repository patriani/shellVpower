#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **trata_entrada(char *entrada);
int confere(char* comando1, char* comando2); 
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
                        confere(comando[i],NULL);
                    }
                    novo[j] = comando[i];
                    novo[j+1] = NULL; // caso seja o único comando recebido o array pode ser passado para execvp
                    j++;
                }
                if(cat == 0 || comando[i+1] == NULL){
                    quit = confere(novo[0],novo[1]); // caso o retorno for 1 o comando já foi executado 
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
int confere(char* comando1, char* comando2){ // duas entradas porque geralmente o cd vem acompanhado com ".." ou com diretorio
    int conf1 = -1; // inicialização != 0
    int conf2 = -1; // inicialização != 0

    printf("comando1 durante: %s\n",comando1);
    printf("comando2 durante: %s\n",comando2);

     conf1 = strcmp(comando1, "quit");
     conf2 = strcmp(comando1, "cd");

    if(conf1 == 0){
        exit(0);
    }
     if(conf2 == 0){
         int comp = strcmp(comando2,"..");
         if(comp == 0){
             chdir("..");
             return 1;
         }
        else if(comando2 != NULL){
            chdir(comando2);
            return 1;
        }
        else{
            printf("\ncd: argumento de CD nao reconhecido");
            return 0;
        }
     }
return 0;
}
