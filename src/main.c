// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include<stdio.h>
#include<string.h>	
#include<stdlib.h>
#include "core/func.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

int main(void){ // não receberemos argumentos da linha de comando


	// LENDO COMANDO E PARSE DO INPUT


	char comando_usuario[500]; // buffer para armazenar o comando do usuario que invoca as funcionalidades
	fgets(comando_usuario, sizeof(comando_usuario), stdin); // fgets é melhor que scanf pois verifica o tamanho do buffer e não para quando encontra o espaço. Inclui \n.
	comando_usuario[strcspn(comando_usuario, "\n")] = 0;
	
	int funcionalidade; // armazena o código de 1 a 14 da funcionalidade que o usuário quer usar
	funcionalidade = atoi(strtok(comando_usuario, " ")); // começa a interpretar o comando: extrai o primeiro trecho delimitado por espaço, e transforma em inteiro
	
	char* token1 = strtok(NULL, " "); // segundo trecho 
	char* token2 = strtok(NULL, " "); // terceiro trecho 
	char* token3 = strtok(NULL, " "); // quarto trecho (se houver)
	char* token4 = strtok(NULL, " "); // quinto trecho (se houver)
	char* token5 = strtok(NULL, " "); // quinto trecho (se houver)

	// DECIDINDO QUAIS ARQUIVOS SERÃO ABERTOS E EM QUE MODO

	FILE* arquivoCSV = NULL;
	FILE* arquivoDados1 = NULL;
	FILE* arquivoDados2 = NULL;
	FILE* arquivoIndice = NULL;
	char* modoCSV = NULL;
	char* modoDados1 = NULL;
	char* modoDados2 = NULL;
	char* modoIndice = NULL;
	char* nomeCSV = NULL;
	char* nomeDados1 = NULL;
	char* nomeDados2 = NULL;
	char* nomeIndice = NULL;
	char* campoOrd = NULL;
	char* campoJuncao1 = NULL;
	char* campoJuncao2 = NULL;

	switch(funcionalidade) {
        case 1:
            modoCSV = "r";
            modoDados1 = "wb+"; // Cria arquivo de dados do zero
            nomeCSV = token1;
            nomeDados1 = token2;
            break;
        case 2:
        case 3:
            modoDados1 = "rb";  // Apenas leitura de dados existente
            nomeDados1 = token1;
            break;
        case 4:
        case 5:
        case 6:
            modoDados1 = "rb+"; // Modifica dados existente
            nomeDados1 = token1;
            break;
        case 7:
            modoDados1 = "rb";  // Leitura de dados existente
            modoIndice = "wb+"; // Cria índice do zero
            nomeDados1 = token1;
            nomeIndice = token2;
            break;
        case 8:
            modoDados1 = "rb";  // Leitura de dados existente
            modoIndice = "rb";  // Leitura de índice existente
            nomeDados1 = token1;
            nomeIndice = token2;
            break;
        case 9:
        case 10:
            modoDados1 = "rb+"; // Modifica dados existente
            modoIndice = "rb+"; // Modifica índice existente
            nomeDados1 = token1;
            nomeIndice = token2;
            break;
        case 11:
        	modoDados1 = "rb";
        	modoDados2 = "rb";
        	nomeDados1 = token1;
        	campoJuncao1 = token2;
        	nomeDados2 = token3;
        	campoJuncao2 = token4;
        	break;
   		case 12:
   			modoDados1 = "rb";
   			modoDados2 = "rb";
   			modoIndice = "rb";
   			nomeDados1 = token1;
   			campoJuncao1 = token2;
   			nomeDados2 = token3;
   			campoJuncao2 = token4;
   			nomeIndice = token5;
   			break;
   		case 13:
   			modoDados1 = "rb";
   			modoDados2 = "wb";
   			nomeDados1 = token1;
   			campoOrd   = token2;
   			nomeDados2 = token3;
   			break;
   		case 14:
   			modoDados1 = "rb+";
   			modoDados2 = "rb+";
   			nomeDados1 = token1;
   			campoJuncao1 = token2;
   			nomeDados2 = token3;
   			campoJuncao2 = token4;
        	break;
        default:
            return 0;
    }

    // ABRINDO OS ARQUIVOS

    if(modoCSV){
    	arquivoCSV = fopen(nomeCSV, modoCSV);
    	if(arquivoCSV == NULL) goto erro;
	}
	
	if(modoDados1){
		arquivoDados1 = abre_binario(nomeDados1, modoDados1);
		if(arquivoDados1 == NULL) goto erro;
	}

	if(modoDados2){
		arquivoDados2 = abre_binario(nomeDados2, modoDados2);
		if(arquivoDados2 == NULL) goto erro;
	}
	
	if(modoIndice){
		arquivoIndice = abrir_indice(nomeIndice, modoIndice);
		if(arquivoIndice == NULL) goto erro;
	}

	// EXECUTANDO
	
	switch(funcionalidade){
		case 1:
			if(arquivoCSV && arquivoDados1)
				if(!func_1(arquivoCSV, arquivoDados1)) goto erro;
			break;
		case 2:
			if(arquivoDados1)
				if(!func_2(arquivoDados1)) goto erro;
			break;
		case 3:
			if(arquivoDados1 && token2){
				if(!func_3(arquivoDados1, atoi(token2))) goto erro;
			}
			break;
		case 4:
			if(arquivoDados1 && token2){
				if(!func_4(arquivoDados1, atoi(token2))) goto erro;
			}
			break;
		case 5:
			if(arquivoDados1 && token2){
				if(!func_5(arquivoDados1, atoi(token2))) goto erro;
			}
			break;
		case 6:
			if(arquivoDados1 && token2){
				if(!func_6(arquivoDados1, atoi(token2))) goto erro;
			}
			break;
		case 7:
			if(arquivoDados1 && arquivoIndice){
				if(!func_7(arquivoDados1, arquivoIndice)) goto erro;
			}
			break;
		case 8:
			if(arquivoDados1 && arquivoIndice && token3){
				if(!func_8(arquivoDados1, arquivoIndice, atoi(token3))) goto erro;
			}
			break;
		case 9:
			if(arquivoDados1 && arquivoIndice && token3){
				if(!func_9(arquivoDados1, arquivoIndice, atoi(token3))) goto erro;
			}
			break;
		case 10:
			if(arquivoDados1 && arquivoIndice && token3){
				if(!func_10(arquivoDados1, arquivoIndice, atoi(token3))) goto erro;
			}
			break;
		case 11:
			if(arquivoDados1 && arquivoDados2){
				if(!func_11(arquivoDados1, campoJuncao1, arquivoDados2, campoJuncao2)) goto erro;
			}
			break;
		case 12:
			if(arquivoDados1 && arquivoDados2){
				if(!func_12(arquivoDados1, campoJuncao1, arquivoDados2, campoJuncao2, arquivoIndice)) goto erro;
			}
			break;
		case 13:
			if(arquivoDados1 && arquivoDados2){
				if(!func_13(arquivoDados1, campoOrd, arquivoDados2)) goto erro;
			}
			break;
		case 14:
			if(arquivoDados1 && arquivoDados2){
				if(!func_14(arquivoDados1, campoJuncao1, arquivoDados2, campoJuncao2)) goto erro;
			}
			break;
		default:
			goto erro;
	}

	// FECHANDO OS ARQUIVOS

	if(arquivoCSV != NULL) fclose(arquivoCSV);
    fecha_binario(arquivoDados1, modoDados1);
    fechar_indice(arquivoIndice, modoIndice);

    // BINÁRIO NA TELA !!!

    if(funcionalidade != 3 && funcionalidade != 2 && funcionalidade != 8 && funcionalidade != 7 && funcionalidade != 11 && funcionalidade != 12 && funcionalidade != 14) {
        BinarioNaTela(nomeDados1);
    }
    if(funcionalidade != 8 && funcionalidade >= 7 && funcionalidade <= 10) {
        BinarioNaTela(nomeIndice);
    }

	return 0;

	erro:

	printf("Falha no processamento do arquivo.\n");
	if(arquivoCSV != NULL) fclose(arquivoCSV);
	fecha_binario(arquivoDados1, modoDados1);
	fechar_indice(arquivoIndice, modoIndice);

	return 0; // para o Makefile não retornar erro no fim da execução, a main deve retornar 0
}
