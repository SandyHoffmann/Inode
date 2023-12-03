#include "user_interaction.h"
#include "super.h"
#include "bitmap.h"
#include "directory.h"
#include "inode.h"
#include "file_operations.h"
#include  <sys/types.h>
#include  <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define SHM_KEY 12345

void user_menu(int fdHd){
    //*key needs to come from file because some systems have a fixed lenghh in shmid and it can retrieve an error when i try to allocate more memory
    key_t key = ftok("output.txt", 'C');


    printf("Welcome to the Inode File System!\n");
    printf("We will format your disk now.\n");
    sleep(1);
    disk_format(fdHd);
    sleep(2);
    printf("Disk formatted!\n");

    printf("Now, choose whats next.\n");

    int option = 0;
    struct SuperBlock ReadBlock = read_superblock(fdHd);
    
    // * Creating shared memory for both parent and child process
    // ! For moment it's set a static size
    size_t size = sizeof(InodeNumberNameDir);

    printf("Size of InodeNumberNameDir: %zu bytes\n", size);
    long shmmax = sysconf(_SC_PAGESIZE) * sysconf(_SC_PHYS_PAGES);

    printf("SHMMAX value on this system: %ld bytes\n", shmmax);
    printf("key: %d\n", key);

    int shmid = shmget(key, size, IPC_CREAT | 0666);
    printf("shmid: %d bytes\n", shmid);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    InodeNumberNameDir *sh_mem = (struct InodeNumberNameDir *) shmat(shmid, NULL, 0);
    char * shared_string;
    // TODO 

    // * Interação com usuário não ser mais por número e sim por comando
    // * - Definir lista de opções possíveis para o usuário
    // * - Então, regra: até comando >>quit() terminal vai estar ativo

    // * Navegação entre diretórios deve estar funcional
    // * - Aproveitar concorrencia implementada até então, mas usa-la de forma mais encapsulada

    // ? Principal
    // * Quando usuário criar arquivo e alocar dados nele, a operação de alocar dados deve estar ocorrendo de forma paralela
    // * - Ou seja, o terminal irá continuar funcionando enquanto os dados estão sendo alocados (pois são operações distintas)
    
    // ? Futuro
    // * Fazer com que a gravação de dados no hd ocorra de forma concorrente
    // * - O que pode se tornar dificil pelo acesso ao arquivo físico (escrita)

    char opcao[255] = "";
    // * Arquivo Fifo em tempo real sobre oq o processo esta fazendo
    // * Basta digitar em um terminal auxiliar: >>mkfifo output >>cat output
    const char *fifoPath = "output";
    int fifo_fd = open(fifoPath, O_WRONLY);

  

    // ? Menu de instruções básico que supõe que:
    // ? - Instruções podem ter mais dados com espaçamento
    // ? - No caso do ls (por enquanto) possui dois dados >> ls "caminho/div/por/barra"

    char *opcAnterior = (char *)malloc(sizeof(char) * 255);
    // * Starting from the root
    long int currentDirectory = 0;
    // * beforeDir
    long int beforeDirectory = 0;
    // * Searching for root
    return_child_inodes(currentDirectory, ReadBlock, fdHd, sh_mem);
    while(strcmp(opcao, "quit")!=0){
        printf(">>");
        fflush(stdout); 
        // expressão para pegar string com espaços para não dar multi insert
        scanf("%[^\n]%*c", opcao);

        char * keys = strtok(opcao, " ");

        if(strcmp(opcao, "ls") == 0){
            // * Listar diretorio
            printf("ls");
                    
            for (int i = 0; i < 64; i++) {
                if (sh_mem->inodeNumbers[i] == 0){
                    break;
                }
                printf(" ~/%s (%ld) (%ld)", sh_mem->dirNames[i], sh_mem->inodeNumbers[i], sh_mem->rootBlocks[i] );
            }


        } else if (strcmp(keys, "cd")  == 0){
            // * Entrar em pasta
            // ! Strtok dentro da função especifica 

            printf("cd");
            keys = strtok(NULL, " ");
            printf("Opc anterior: %s\n", keys);
            printf("Caminho: %s\n", opcao);
            // ! Apenas voltando uma vez por momento
            if (strcmp(keys, "..") == 0){
                memcpy(sh_mem, return_child_inodes(beforeDirectory, ReadBlock, fdHd, sh_mem), sizeof(struct InodeNumberNameDir));
                currentDirectory = beforeDirectory;
            } else {

                // * Achar nome na lista sh_mem, para pegar o id;

                long int id_bloco = -1;

                for (int i = 0; i < 64; i++) {
                    if (sh_mem->rootBlocks[i] == 0){
                        break;
                    } else if (strcmp(sh_mem->dirNames[i], keys) == 0){
                        id_bloco = sh_mem->rootBlocks[i];
                        break;
                    }
                }

                if (id_bloco < 0){
                    printf("Caminho inválido!\n");
                } else {
                    beforeDirectory = currentDirectory;
                    printf("id blocoNumber: %ld\n", id_bloco);
                    memcpy(sh_mem, return_child_inodes(id_bloco, ReadBlock, fdHd, sh_mem), sizeof(struct InodeNumberNameDir));
                    currentDirectory = id_bloco;
                }
            }
        } else if (strcmp(opcao, "touch")  == 0){ 
            setbuf(stdout, NULL); 
            printf("touch");
            keys = strtok(NULL, " ");
            pid_t process_file = fork();

            if (process_file == 0){
                if (dup2(fifo_fd, STDOUT_FILENO) == -1) {
                    exit(EXIT_FAILURE);
                }
                printf("Currentdir: %d\n", currentDirectory);

                allocate_dir_v2(fdHd, ReadBlock,keys,"inputTest.txt",currentDirectory);

                fflush(stdout); 
                if (dup2(STDOUT_FILENO, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                memcpy(sh_mem, return_child_inodes(currentDirectory, ReadBlock, fdHd, sh_mem), sizeof(struct InodeNumberNameDir));

                exit(EXIT_SUCCESS);
            }

        } else if (strcmp(opcao, "cat")  == 0){
            printf("cat");
            keys = strtok(NULL, " ");
            long int id_inode = -1;

            for (int i = 0; i < 64; i++) {
                if (sh_mem->inodeNumbers[i] == 0){
                    break;
                } else if (strcmp(sh_mem->dirNames[i], keys) == 0){
                    id_inode = sh_mem->inodeNumbers[i];
                    break;
                }
            }

            if (id_inode < 0){
                printf("Caminho inválido!\n");
            } else {
                printf("id id_inode: %ld\n", id_inode);
                read_inode(fdHd, ReadBlock, id_inode);
                keys = strtok(NULL, " ");
                if (strcmp(keys, "y") == 0){
                    printf("Showing a file content.\n");
                    read_inode_data(fdHd, ReadBlock, id_inode);
                }
            }
        }
        else {
            printf("Comando inválido");
        }
        printf("\n");

    }
    // Fechando o fifo

    close(fifo_fd);

}


/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 23/04/2023.
 * \brief Formating a disk in Inode system means:
 * 1. Creating a superblock.
 * 1.1 Superblock will contain all the information about the disk.
 * 1.2 Superblock will be the first block of the disk.
 * 2. Allocate the first directory entry (root directory) = /.
 * 2.1 Allocate the inode for the root directory.
 * 3. Allocating the inode bitmap including this new instance of root directory.
 * 4. Allocating the data bitmap including the superblock location, inode bitmap, data bitmap, the root directory, and inode reserved sector.
 * \param fdhd The file containing the hd itself.
 **/
void disk_format(int fdHd)
{
    // * Inode size increased to 128 bytes (22/06/2023)
    struct SuperBlock block = create_super_block(4096, 128, 1);
    void *buffer = malloc(block.block_size);
    memcpy(buffer, &block, sizeof(struct SuperBlock));
    lseek(fdHd, 0, SEEK_SET);
    write(fdHd, buffer, block.block_size);
    free(buffer);

    // * Inode bitmap already mounted on hd
    int bit_free_inode = return_free_inode_bit(fdHd, block);

    Directory *root = create_dir("/", bit_free_inode);
    struct Inode *inode = create_inode("", 1, NULL);

    // * Write Inode in HD
    long int adress_first_inode = block.inode_start;
    lseek(fdHd, adress_first_inode, SEEK_SET);
    write(fdHd, inode, block.inode_size);

    // * Write Directory in HD
    long int adress_dir = block.inode_directory_start;
    lseek(fdHd, adress_dir, SEEK_SET);
    write(fdHd, root, sizeof(Directory));

    // * Data bitmap represents the data blocks that are free or not.

    long int total_data = block.data_start;                     // * bytes 0 - 16527862 byte
    int occupied_blocks = ROUND(total_data / block.block_size); // * blocks = 4036 occupied blocks


    for (int i = 0; i < occupied_blocks; i++)
    {
        return_free_data_bit(fdHd, block);
    }

    printf("Occupied blocks: %d\n", occupied_blocks);

    // for (int i = 0; i < total_blocks_full_byte + 1; i++){
    //     printf("Byte 2: %x\n", list_data_byte[i]);
    // }

    // list_data_bits[total_data_bits - 1] = resto;

    // * 11111111110000000000000000000000000000000000
    // * 1 byte = 8 bits
    // * 1 byte completo = 11111111
    // write(fdHd, list_data_bits, total_data_bits);
}
