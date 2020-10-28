#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **trata_entrada(char *entrada) {
    char *parsed; // recebe comando isolado
    char **commando; // alocação dinamica que cria um vetor de endereços
    int  cont = 0;  
    char *separador = " "; // define um char separador de comandos

    commando = malloc(8 * sizeof(char *)); 

    parsed = strtok(entrada, separador); // leitura começa na entrada e vai até um " "
    while (parsed != NULL) { // caso o retorno de strtok seja !=NULL (retornou um ponteiro para string terminada em NULL)
        commando[cont] = parsed; // posição cont de commando recebe endereço do elemento apontado por parsed
        cont++;

        parsed = strtok(NULL, separador); // reinicia parsed
    }

    commando[cont] = NULL; // se entrou no while cont != 0, caso contrário o retorno eh NULL
    return commando;
}

int main() {
    char **comando; 
    char *entrada; // leitura bruta da entrada 
    pid_t pid_filho;
    int wstatus; // armazenará status do processo filho
    
    
    while(1){
        entrada = readline("shVpower> "); // printa esse texto e lê o que vier depois dele 
        comando = trata_entrada(entrada); // recebe a entrada bruto e retorna o comando legível para execvp()
        if(comando[0] == NULL){ // se a lista de comandos estiver vazia [...]
            free(comando);
            free(entrada); 
            printf("erro: Nenhum comando inserido\n");
            continue;
        }else{
        pid_filho = fork(); // cria processo para executar comando
        if (pid_filho == 0) { // pid == 0 quando eh criado corretamente
            execvp(comando[0], comando);
            printf("This won't be printed if execvp is successul\n");
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
        entrada = NULL;
        free(comando);
        }
    }
}

/* w = waitpid(...) : 
    Em caso de sucesso w é o pid do processo que teve seu estado alterado
    código de erro == -1 
*/