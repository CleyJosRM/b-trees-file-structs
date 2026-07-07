// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

bool func_14(FILE* arquivoDados1, char* campoJuncao1, FILE* arquivoDados2, char* campoJuncao2){

	// Verificando campos de junção

	if(strcmp(campoJuncao1, "codProxEstacao") != 0 || strcmp(campoJuncao2, "codEstacao") != 0){
		DEBUG("ERRO EM func_14: CAMPOS DE JUNÇÃO INVÁLIDOS.")
		return false;
	}

	// Carregando os registros dos dois arquivos para a memória principal

	int i1 = 0;
	int i2 = 0;
	int tam_array1 = 32;
	int tam_array2 = 32;
	REG_DADOS_STRUCT** registros1 = (REG_DADOS_STRUCT**)malloc(tam_array1 * sizeof(REG_DADOS_STRUCT*));
	REG_DADOS_STRUCT** registros2 = (REG_DADOS_STRUCT**)malloc(tam_array2 * sizeof(REG_DADOS_STRUCT*));
	if(registros1 == NULL || registros2 == NULL){
		DEBUG("ERRO EM func_14: ALOCAÇÃO DE ARRAYS FALHOU.")
		free(registros1);
		free(registros2);
		return false;
	}

	// Lendo e filtrando o arquivo 1

	fseek(arquivoDados1, HEADER_S, SEEK_SET);
	while(1){
		registros1[i1] = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
		if(registros1[i1] == NULL){
			DEBUG("ERRO EM func_14: ALOCAÇÃO DE registros1[%d] FALHOU.\n", i1);
			goto erro;
		}

		if(load_registro(arquivoDados1, registros1[i1]) == false){
			free(registros1[i1]);
			break;
		}else if(registros1[i1]->removido == '1' || registros1[i1]->codProxEstacao == -1){
			liberar_registro(registros1[i1]);
			continue;
		}

		i1++;
		if(i1 == tam_array1){
			tam_array1 *= 2;
			REG_DADOS_STRUCT** temp = (REG_DADOS_STRUCT**)realloc(registros1, tam_array1 * sizeof(REG_DADOS_STRUCT*));
			if(temp == NULL){
				DEBUG("ERRO EM func_14: REALOCAÇÃO DE registros1 FALHOU.\n");
				goto erro;
			}
			registros1 = temp;
		}
	}

	// Lendo e filtrando o arquivo 2

	fseek(arquivoDados2, HEADER_S, SEEK_SET);
	while(1){
		registros2[i2] = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
		if(registros2[i2] == NULL){
			DEBUG("ERRO EM func_14: ALOCAÇÃO DE registros2[%d] FALHOU.\n", i2);
			goto erro;
		}

		if(load_registro(arquivoDados2, registros2[i2]) == false){
			free(registros2[i2]);
			break;
		}else if(registros2[i2]->removido == '1' || registros2[i2]->codEstacao == -1){
			liberar_registro(registros2[i2]);
			continue;
		}

		i2++;
		if(i2 == tam_array2){
			tam_array2 *= 2;
			REG_DADOS_STRUCT** temp = (REG_DADOS_STRUCT**)realloc(registros2, tam_array2 * sizeof(REG_DADOS_STRUCT*));
			if(temp == NULL){
				DEBUG("ERRO EM func_14: REALOCAÇÃO DE registros2 FALHOU.\n");
				goto erro;
			}
			registros2 = temp;
		}
	}

	// Ordenando os dois conjuntos de registros

	qsort(registros1, i1, sizeof(REG_DADOS_STRUCT*), comparar_registros_por_codProxEstacao);
	qsort(registros2, i2, sizeof(REG_DADOS_STRUCT*), comparar_registros_por_codEstacao);

	// Realizando a autojunção por ordenação-intercalação

	int p1 = 0;
	int p2 = 0;
	while(p1 < i1 && p2 < i2){
		int chave1 = registros1[p1]->codProxEstacao;
		int chave2 = registros2[p2]->codEstacao;

		if(chave1 < chave2){
			p1++;
			continue;
		}
		if(chave1 > chave2){
			p2++;
			continue;
		}

		int fim1 = p1;
		int fim2 = p2;
		while(fim1 < i1 && registros1[fim1]->codProxEstacao == chave1) fim1++;
		while(fim2 < i2 && registros2[fim2]->codEstacao == chave2) fim2++;

		for(int a = p1; a < fim1; a++){
			for(int b = p2; b < fim2; b++){
				printf("%d %s %s %d %s\n", registros1[a]->codEstacao, registros1[a]->nomeEstacao, registros1[a]->nomeLinha, registros1[a]->codProxEstacao, registros2[b]->nomeEstacao);
			}
		}

		p1 = fim1;
		p2 = fim2;
	}

	// Liberando memória

	for(int j = 0; j < i1; j++) liberar_registro(registros1[j]);
	for(int j = 0; j < i2; j++) liberar_registro(registros2[j]);
	free(registros1);
	free(registros2);

	return true;

	erro:

	for(int j = 0; j < i1; j++) liberar_registro(registros1[j]);
	for(int j = 0; j < i2; j++) liberar_registro(registros2[j]);
	if(registros1 != NULL) free(registros1);
	if(registros2 != NULL) free(registros2);

	return false;
}