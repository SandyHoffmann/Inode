# Inode

Sistema que simula a parte de gerenciamento de arquivos de um sistema operacional. Possui uma divisão semelhante a figura abaixo:

Para mais informações acessar: [LINK DO ARQUIVO]

## Inicialização

Criar HD virtual utilizando o comando:
```
dd if=/dev/zero of=bla.hd bs=GB count=2
```
Criar um fifo para ver o processo do paralelismo
```
mkfifo output
```
Compilar o código
```
gcc main.c -lm
```

## Rodar o código
Abra dois terminais, em um digite:
```
./a.out
```
No outro abra o fifo
```
cat output
```
Conforme figura abaixo:

## Comandos disponíveis
- `ls`: Listar diretórios na árvore atual
- `touch nomeArquivo`: Cria pasta que contém arquivo
- `cat nomeArquivo s`: Mostra metadados do arquivo (`s`: mostra conteudo | `n`: não mostra conteudo)
- `cd nomeArquivo`: Navega para o diretório (ls muda também, e os arquivos vão ser criados apartir daquele diretório)
- `quit`: Sair

---
## Autores
- Sandy Hoffmann @SandyHoffmann
- Leonardo de Souza Fiamoncini @LeonardoFiamoncini
