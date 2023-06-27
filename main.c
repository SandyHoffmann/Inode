#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "super.c"
#include "bitmap.c"
#include "directory.c"
#include "inode.c"
#include "file_operations.c"
#include "user_interaction.c"

#include <unistd.h>

/**

* ? Important notes:

Criar HD = dd if=/dev/zero of=bla.hd bs=GB count=2

Hexdump hexdump -n512 -s60 -c bla.hd
-n = number of bytes to dump (bytes)
-s = offset to start dumping from (bytes)

xxd -s 4594 -l 512 -b bla.hd
-s = start
 -l = quantity

Making a big txt for testing:
https://www.browserling.com/tools/text-repeat

*/

int fdHd;

void open_hd();
void close_hd();


void resetHdF()
{
    char buffer[4096];
    bzero(buffer, 4096);
    lseek(fdHd, 0, SEEK_SET);
    int totalHd1gb = (1024 * 1024 * 1024) / 4096;
    for (int i = 0; i < totalHd1gb; i++)
    {
        write(fdHd, buffer, 4096);
    }
}

void directory_test(int fdHd)
{
    resetHdF();
    sleep(2);
    disk_format(fdHd);
    sleep(2);

    struct SuperBlock ReadBlock;
    char *buffer = malloc(4096);
    lseek(fdHd, 0, SEEK_SET);
    read(fdHd, buffer, 4096);
    memcpy(&ReadBlock, buffer, sizeof(struct SuperBlock));

    create_dump_directory_tree(fdHd, ReadBlock);
}

void logo_print(){
    printf("\033[0;32;40m"); // Configura a fonte verde e o fundo preto
    printf("      :::::::::::       ::::    :::       ::::::::       :::::::::       ::::::::::        ::::::::    :::   :::       ::::::::   :::::::::::       ::::::::::         :::   ::: \n");
    printf("         :+:           :+:+:   :+:      :+:    :+:      :+:    :+:      :+:              :+:    :+:   :+:   :+:      :+:    :+:      :+:           :+:               :+:+: :+:+: \n");
    printf("        +:+           :+:+:+  +:+      +:+    +:+      +:+    +:+      +:+              +:+           +:+ +:+       +:+             +:+           +:+              +:+ +:+:+ +:+ \n");
    printf("       +#+           +#+ +:+ +#+      +#+    +:+      +#+    +:+      +#++:++#         +#++:++#++     +#++:        +#++:++#++      +#+           +#++:++#         +#+  +:+  +#+  \n");
    printf("      +#+           +#+  +#+#+#      +#+    +#+      +#+    +#+      +#+                     +#+      +#+                +#+      +#+           +#+              +#+       +#+   \n");
    printf("     #+#           #+#   #+#+#      #+#    #+#      #+#    #+#      #+#              #+#    #+#      #+#         #+#    #+#      #+#           #+#              #+#       #+#    \n");
    printf("###########       ###    ####       ########       #########       ##########        ########       ###          ########       ###           ##########       ###       ###   \n");
    printf("\033[0m"); // Restaura as configurações de cores padrão

}

void main()
{
    logo_print();
    open_hd();
    resetHdF();
    sleep(2);
    user_menu(fdHd);
    // directory_test(fdHd);

    close_hd();
}

void open_hd()
{
    printf("Abrindo HD...");
    if ((fdHd = open("bla.hd", O_RDWR)) == -1)
    {
        printf("ERRO");
        exit(1);
    }
}

void close_hd()
{
    close(fdHd);
}


