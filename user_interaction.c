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
    //key needs to come from file because some systems have a fixed lenghh in shmid and it can retrieve an error when i try to allocate more memory
    key_t key = ftok("output.txt", 'B');


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
                        
            // InodeNumberNameDir *sh_mem_ = (struct InodeNumberNameDir *) shmat (shmid, NULL, 0);

            // if ((sh_mem_ = shmat (shmid, NULL, 0)) == (void *)-1){
            //         perror("acoplamento impossivel") ;   
            // }

            // if (sh_mem_ == NULL) {
            //     perror("Failed to attach shared memory");
            //     exit(1);
            // }

            printf("-----%s",sh_mem->dirNames[0]);
            printf("-----%ld",sh_mem->inodeNumbers[0]);

            // shmdt(sh_mem_);
            // printf("-----%s",sh_mem_->dirNames[1]);

            // printf("-----%ld",sh_mem_->inodeNumbers[1]);

            // for (int i = 0; i < 64; i++) {
            //     if (sh_mem_->inodeNumbers[i] == 0){
            //         break;
            //     }
            //     printf(" ~/%s (%ld)", sh_mem_->dirNames[i], sh_mem_->inodeNumbers[i] );
            // }


        } else if (strcmp(keys, "cd")  == 0){
            // * Entrar em pasta
            // ! Strtok dentro da função especifica 

            printf("cd");
            keys = strtok(NULL, " ");
            printf("Opc anterior: %s\n", keys);
            printf("Caminho: %s\n", opcao);

        } else if (strcmp(opcao, "touch")  == 0){ 
            setbuf(stdout, NULL); 
            printf("touch");
            pid_t process_file = fork();
            if (process_file == 0){
                if (dup2(fifo_fd, STDOUT_FILENO) == -1) {
                    exit(EXIT_FAILURE);
                }
                allocate_dir_v2(fdHd, ReadBlock, "/pasta1","inputTest.txt");

                fflush(stdout); 
                if (dup2(STDOUT_FILENO, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                memcpy(sh_mem, return_child_inodes(currentDirectory, ReadBlock, fdHd, sh_mem), sizeof(struct InodeNumberNameDir));
                // for (int i = 0; i < 64; ++i) {
                //     // Check for memory allocation errors...
                //     if (sh_mem->dirNames[i] == NULL) {
                //         break;
                //         // Handle error: free previously allocated memory and exit or return an error code
                //     }
                //     strcpy(sh_mem->dirNames[i], sh_mem->dirNames[i]);
                //     printf("-----%s",sh_mem->dirNames[0]);

                // }
                
               // sh_mem->dirNames[0] = "aaaaaaaaaaaaaaaaaaaa";
                // char stringgggggg[22] = "aaaa";
                // strcpy(sh_mem->dirNames[0], stringgggggg);
                // char *shared_message = (char *)shmat(shmid, NULL, 0);
                // strncpy(shared_message, stringgggggg, 255 - 1);

                InodeNumberNameDir *sh_mem = (struct InodeNumberNameDir *) shmat(shmid, NULL, 0);
                // memcpy(sh_mem, return_child_inodes(currentDirectory, ReadBlock, fdHd, sh_mem), sizeof(struct InodeNumberNameDir));
                //sh_mem = return_child_inodes(currentDirectory, ReadBlock, fdHd, sh_mem);
                for (int i = 0; i < 1; ++i) {
                    // Check for memory allocation errors...
                    if (sh_mem->dirNames[i] == NULL) {
                        break;
                        // Handle error: free previously allocated memory and exit or return an error code
                    }
                    // sh_mem->dirNames[i] = sh_mem->dirNames[i];
                    // snprintf(sh_mem->dirNames[i], sizeof(inodeNames[i]), inodeNames[i], i);
                    // memmove(sh_mem->dirNames[i], "aaaaaaaaaaaaa", 255);
                    printf("-----%s",sh_mem->dirNames[0]);

                }
                // sh_mem->dirNames =
                printf("-----%s",sh_mem->dirNames[0]);

                exit(EXIT_SUCCESS);
            }

        } else {
            printf("Comando inválido");
        }
        // strcpy(opcAnterior, keys);
        printf("\n");

    }
    // Fechando o fifo

    close(fifo_fd);



    while (option != 5){
        printf("\n*--------------------------------*\n");
        printf("1. Show the superblock info.\n");
        printf("2. Create a file.\n");
        printf("3. List all files and directories.\n");
        printf("4. Show a file content.\n");
        printf("5. LS.\n");
        printf("6. EXIT.\n");
        scanf("%d", &option);


        switch (option){
            case 1:
                printf("Showing info.\n");
                show_superblock(fdHd);
                break;
            case 2:
                printf("Creating a dumb directory three.\n");
                // * Proccess responsible for managing inode directories through the volatil memory, for fast access
                pid_t manager_dir_mem = fork();
                if (manager_dir_mem == 0){
                    printf("------------------------\n");
                    printf("PROCESSO FILHO\n");
                    printf("------------------------\n");
                    // * Creating dump data
                    create_dump_directory_tree(fdHd, ReadBlock);
                }  
                printf("------------------------\n");
                printf("PROCESSO PAI\n");
                printf("------------------------\n");
                // * Waiting for creation
                waitpid(manager_dir_mem, NULL, 0);
               
                printf("shmid %d\n", shmid);

                printf("Processo pai\n");
                long int inode_number_2 = 0;
                // * Catching inodes from hd of the current directory (in this case, is the root directory)
                InodeNumberNameDir * father_addresses;
                int arrSize = sizeof father_addresses / sizeof father_addresses[0];
                // ! STATIC SIZE (for moment)
                for (int i = 0; i < 64; i++) {
                    if (father_addresses->inodeNumbers[i] == 0){
                        break;
                    }
                    printf("~/%s (%ld)", father_addresses->dirNames[i], father_addresses->inodeNumbers[i] );
                }
                // * 
                InodeNumberNameDir *sh_mem;
                // * Attaching the shared memory to the process, to allow the operation of moving the data
                if ((sh_mem = shmat (shmid, 0, 0)) == (InodeNumberNameDir*)-1){
                        perror("acoplamento impossivel") ;
                        exit (1) ;
                    }
                // * Moving the actual data from buffer to shared memory
                memmove(sh_mem, father_addresses, sizeof(father_addresses)+1);


                break;
            case 3:
                printf("Listing all files and directories.\n");
                show_all_directories(fdHd, ReadBlock);
                break;
            case 4:
                printf("Give me the inode number: \n");
                long int inode_number = 0;
                scanf("%ld", &inode_number);
                read_inode(fdHd, ReadBlock, inode_number);
                printf("Show data? (y/n)\n");
                char show_data = 'n';
                scanf(" %c", &show_data);
                if (show_data == 'y'){
                    printf("Showing a file content.\n");
                    read_inode_data(fdHd, ReadBlock, inode_number);
                }
                break;
            case 5:
                // * LS Function
                printf("ls");
                InodeNumberNameDir *sh_mem_;

                if ((sh_mem_ = shmat (shmid, 0, 0)) == (InodeNumberNameDir*)-1){
                        perror("acoplamento impossivel") ;
                        
                }
                printf("-----%ld",sh_mem_->inodeNumbers[0]);
                printf("-----%ld",sh_mem_->inodeNumbers[1]);
                printf("-----%ld",sh_mem_->inodeNumbers[2]);
                printf("-----%ld",sh_mem_->inodeNumbers[3]);

                printf("\n");


                for (int i = 0; i < 64; i++) {
                    if (father_addresses->inodeNumbers[i] == 0){
                        break;
                    }
                    printf(" ~/%s (%ld)", father_addresses->dirNames[i], father_addresses->inodeNumbers[i] );
                }
                break;
            
            case 6:
                // * Criar arquivo com dados de input
                
            case 7:
                // shmdt(sh_mem);
                shmctl(shmid, IPC_RMID, 0);

                printf("Exiting.\n");
                exit (1);
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
        printf("\n*--------------------------------*\n");

    }
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
