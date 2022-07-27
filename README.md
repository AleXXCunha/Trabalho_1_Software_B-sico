Trabalho 1 da Disciplina de Software Básico, no qual consiste em um montador de 2 passadas com pré-processamento de diretivas e macros em C++ para códigos em Pseudo-Assembly, definido em aula.

O código foi desenvolvido no Windows 11 utilizando o gcc/g++ versão 6.3.0 (MinGW). Para compilar o código basta escrever no terminal:
g++ Trabalho_1_Alexandre_Cunha_180041169.cpp -o montador

Isso gerará um arquivo executável chamado montador que também se encontra em anexo no arquivo .zip do trabalho. Depois disso, o funcionamento do programa é exatamente igual o informado nas intruções do trabalho digitando-se no terminal:
 ./montador -p myprogram.asm saida.obj

Podendo escolher o modo de funcionamento do montador entre -p, -m e -o, "myprogram.asm" sendo o arquivo de entrada e "saida.obj" o arquivo de saída.
