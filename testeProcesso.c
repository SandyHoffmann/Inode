#include <stdio.h>
#include <stdlib.h>
// Necessária por conter a definição de fork.
#include <unistd.h>
// Necessária por conter a definição o tipo pid_t, usado para armazenar o ID de um processo.
#include <sys/types.h>
#include <sys/stat.h>
// Necessária por conter a definição de waitpid, usada para esperar pelo término de um processo filho específico ou de qualquer processo filho, dependendo dos parâmetros.
#include <sys/wait.h>
// Necessária por conter a definição da função open() para criar um arquivo.
#include <fcntl.h>

int main() {
    pid_t pid1, pid2;

    // Primeiro processo filho.
    pid1 = fork();

    if (pid1 < 0) {
        perror("Erro ao criar o primeiro processo filho");
        return 1;
    }
    else if (!pid1) {
        // Código do primeiro processo filho.
        printf("Este é o primeiro processo filho! PID do processo: %d\n", getpid());
        
        // Cria um arquivo.
        int fd = open("arquivo.txt", O_WRONLY | O_CREAT, 0644);
        if (fd < 0) {
            perror("Erro ao criar o arquivo");
            return 1;
        }
        
        char text[100];

        // Usando sprintf para formatar a string.
        sprintf(text, "O processo %d é muito daora porque ele me deixa escrever isso no arquivo.\n", getpid());

        // Escreve no arquivo.
        write(fd, text, 77);

        // Fecha o arquivo.
        close(fd);
        
        return 0;
    }

    // Processo pai continua aqui.

    // Segundo processo filho.
    pid2 = fork();

    if (pid2 < 0) {
        perror("Erro ao criar o segundo processo filho");
        return 1;
    }
    else if (!pid2) {
        // Código do segundo processo filho.
        printf("Este é o segundo processo filho! PID do processo: %d\n", getpid());

        // Abrir o terminal
        execlp("gnome-terminal", "gnome-terminal", NULL);

        // Esta parte só será executada se ocorrer um erro ao executar o terminal.
        perror("Erro ao abrir o terminal");
        return 1;
    }

    // Processo pai continua aqui.

    // Espera os processos filhos terminarem.
    // Essas chamadas bloqueiam a execução do processo pai até que os processos filho terminem.
    // Quando os processos filho terminam, o processo pai continua a execução após essas linhas de código.
    // Dessa forma, sincronizamos a execução dos processos pai e filhos.
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Processo pai! Meu PID é: %d\n", getpid());

    return 0;
}