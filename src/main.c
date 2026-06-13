// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include<stdio.h>
#include<string.h>	
#include<stdlib.h>
#include "core/func.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

int main(void){ // não receberemos argumentos da linha de comando

	char comando_usuario[500]; // buffer para armazenar o comando do usuario que invoca as funcionalidades
	fgets(comando_usuario, sizeof(comando_usuario), stdin); // fgets é melhor que scanf pois verifica o tamanho do buffer e não para quando encontra o espaço. Inclui \n.
	comando_usuario[strcspn(comando_usuario, "\n")] = 0;
	
	int funcionalidade; // armazena o código de 1 a 6 da funcionalidade que o usuário quer usar
	funcionalidade = atoi(strtok(comando_usuario, " ")); // começa a interpretar o comando: extrai o primeiro trecho delimitado por espaço, e transforma em inteiro
	
	char* token1 = strtok(NULL, " ");
	char* token2 = strtok(NULL, " ");
	char* token3 = strtok(NULL, " ");

	FILE* arquivoCSV = NULL;
	FILE* arquivoDados = NULL;
	FILE* arquivoIndice = NULL;
	char* modoCSV = NULL;
	char* modoDados = NULL;
	char* modoIndice = NULL;
	char* nomeCSV = NULL;
	char* nomeDados = NULL;
	char* nomeIndice = NULL;

	switch(funcionalidade) {
        case 1:
            modoCSV = "r";
            modoDados = "wb+"; // Cria arquivo de dados do zero
            nomeCSV = token1;
            nomeDados = token2;
            break;
        case 2:
        case 3:
            modoDados = "rb";  // Apenas leitura de dados existente
            nomeDados = token1;
            break;
        case 4:
        case 5:
        case 6:
            modoDados = "rb+"; // Modifica dados existente
            nomeDados = token1;
            break;
        case 7:
            modoDados = "rb";  // Leitura de dados existente
            modoIndice = "wb+"; // Cria índice do zero
            nomeDados = token1;
            nomeIndice = token2;
            break;
        case 8:
            modoDados = "rb";  // Leitura de dados existente
            modoIndice = "rb";  // Leitura de índice existente
            nomeDados = token1;
            nomeIndice = token2;
            break;
        case 9:
        case 10:
            modoDados = "rb+"; // Modifica dados existente
            modoIndice = "rb+"; // Modifica índice existente
            nomeDados = token1;
            nomeIndice = token2;
            break;
        default:
            return 1;
    }

    if(modoCSV){
    	arquivoCSV = fopen(nomeCSV, modoCSV);
    	if(arquivoCSV == NULL) goto erro;
	}
	
	if(modoDados){
		arquivoDados = abre_binario(nomeDados, modoDados);
		if(arquivoDados == NULL) goto erro;
	}
	
	if(modoIndice){
		arquivoIndice = abrir_indice(nomeIndice, modoIndice);
		if(arquivoIndice == NULL) goto erro;
	}

	// EXECUTANDO
	
	switch(funcionalidade){
		case 1:
			if(arquivoCSV && arquivoDados)
				if(!func_1(arquivoCSV, arquivoDados)) goto erro;
			break;
		case 2:
			if(arquivoDados)
				if(!func_2(arquivoDados)) goto erro;
			break;
		case 3:
			if(arquivoDados && token2){
				if(!func_3(arquivoDados, atoi(token2))) goto erro;
			}
			break;
		case 4:
			if(arquivoDados && token2){
				if(!func_4(arquivoDados, atoi(token2))) goto erro;
			}
			break;
		case 5:
			if(arquivoDados && token2){
				if(!func_5(arquivoDados, atoi(token2))) goto erro;
			}
			break;
		case 6:
			if(arquivoDados && token2){
				if(!func_6(arquivoDados, atoi(token2))) goto erro;
			}
			break;
		case 7:
			if(arquivoDados && arquivoIndice){
				if(!func_7(arquivoDados, arquivoIndice)) goto erro;
			}
			break;
		case 8:
			if(arquivoDados && arquivoIndice && token3){
				if(!func_8(arquivoDados, arquivoIndice, atoi(token3))) goto erro;
			}
			break;
		case 9:
			if(arquivoDados && arquivoIndice && token3){
				if(!func_9(arquivoDados, arquivoIndice, atoi(token3))) goto erro;
			}
			break;
		case 10:
			if(arquivoDados && arquivoIndice && token3){
				if(!func_10(arquivoDados, arquivoIndice, atoi(token3))) goto erro;
			}
			break;
	}

	// Fechando os arquivos

	if(arquivoCSV != NULL) fclose(arquivoCSV);
    fecha_binario(arquivoDados, modoDados);
    fechar_indice(arquivoIndice, modoIndice);

    // Binário na tela !!!

    if(funcionalidade != 8 && funcionalidade != 7) {
        BinarioNaTela(nomeDados);
    }
    if(funcionalidade != 8 && funcionalidade >= 7) {
        BinarioNaTela(nomeIndice);
    }

	return 0;

	erro:

	printf("Falha no processamento do arquivo.\n");
	if(arquivoCSV != NULL) fclose(arquivoCSV);
	fecha_binario(arquivoDados, modoDados);
	fechar_indice(arquivoIndice, modoIndice);

	return 0;
}
