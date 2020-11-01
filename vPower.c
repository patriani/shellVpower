#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **trata_entrada(char *entrada);
int confere(char* comando); 

int main() {
    char **comando; 
    char *entrada; // leitura bruta da entrada 
    pid_t pid_filho;
    int wstatus; // armazenará status do processo filho
    
    
    while(1){
        entrada = readline("shVpower> "); // printa esse texto e lê o que vier depois dele 
        comando = trata_entrada(entrada); // recebe a entrada bruto e retorna array de comandos legível por execvp()

        confere((*comando)); // verifica se eh um comando de saída ou cd
        
        if(!comando[0]){ // não aponta para nenhum comando[...]
            printf("erro: Nenhum comando inserido\n");
            free(comando);
            free(entrada); 
            continue; // próxima iteração e pede entrada novamente
        }else{
            pid_filho = fork(); // cria processo para executar comando
        if (pid_filho == 0) { // pid == 0 quando eh criado corretamente
            execvp(comando[0], comando);
        }else if(pid_filho == -1){ // 
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
        //fpurge(stdin);
        entrada = NULL;
        free(comando);
        }
    }
}

char **trata_entrada(char *entrada) {
    char *parsed; // recebe comando isolado
    char **commando; // alocação dinamica que cria um vetor de endereços2
    int  cont = 0;  
    char *separador = " "; // define um char separador de comandos
    int temp;
    commando = malloc(8 * sizeof(char *));

    parsed = strtok(entrada, separador); // leitura começa na entrada e vai até um " "

    while (parsed != NULL) { // caso o retorno de strtok seja !=NULL (retornou um ponteiro para string terminada em NULL)
        temp = strcmp(parsed,";");
        if(temp != 0){ //não armazena o char de separação de comandos (";")
            commando[cont] = parsed; // posição cont de commando recebe endereço do elemento apontado por parsed
            printf("comando[%d] == %s",cont,commando[cont]);
            cont++;
        }
        parsed = strtok(NULL, separador); // demais pesquisas por " " (faz essa separação até que encontre o final do vetor)
    }
    commando[cont] = NULL; // se entrou no while cont != 0, caso contrário o retorno eh NULL
    return commando;
}

int confere(char* comando){
    int conf1 = -1; // inicialização != 0
    int conf2 = -1; // inicialização != 0

    conf1 = strcmp(comando, "quit");
    conf2 = strcmp(comando, "cd");

    if(conf1 == 0){
        exit(0);
    }
    if(conf2 == 0){
        char* temp = &comando[1];
        int str1 = strcmp(temp,"..");
        if(str1 == 0){
            chdir("..");
        }
        else{
            chdir(comando);
        }
    }
return 1;
}
