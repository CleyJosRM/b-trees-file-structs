// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

bool func_12(FILE* arquivoDados1, char* campoJuncao1, FILE* arquivoDados2, char* campoJuncao2, FILE* arquivoIndice){

	// Verificando campos de junção

	if(strcmp(campoJuncao1, "codProxEstacao") != 0 || strcmp(campoJuncao2, "codEstacao") != 0){
		DEBUG("ERRO EM func_12: CAMPOS DE JUNÇÃO INVÁLIDOS.");
		return false;
	}

	// Alocando memória para dois registros

	REG_DADOS_STRUCT* registroDados1 = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
	REG_DADOS_STRUCT* registroDados2 = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
	if( !registroDados1 || !registroDados2 ){
		free(registroDados1);
		free(registroDados2);
		return false;
	}
	
	// Iterando sobre o arquivo 1:

	fseek(arquivoDados1, HEADER_S, SEEK_SET);

	while (load_registro(arquivoDados1, registroDados1) != false){

		// Caso o registro do arquivo 1 seja removido ou tenha codProxEstacao -1, pula (otimização)
		if(registroDados1->removido == '1' || registroDados1->codProxEstacao == -1){
			goto continua_1;
		}

		// Buscando no índice:

		long int byteoffset = buscar_entrada(arquivoIndice, registroDados1->codProxEstacao);

		if(byteoffset == -1){ // -1 é o código de erro do índice
			DEBUG("Não existe estação com codEstacao %d\n", registroDados1->codProxEstacao);
			goto continua_1;
		}

		// Obtendo o arquivo 2

		fseek(arquivoDados2, byteoffset, SEEK_SET);

		if(load_registro(arquivoDados2, registroDados2) == false){
			goto erro;
		}
		
		// Caso o registro do arquivo 2 esteja removido, pula
		if(registroDados2->removido == '1'){
			goto continua_2;
		}

		// Imprimindo 
		printf("%d %s %s %d %s\n", registroDados1->codEstacao, registroDados1->nomeEstacao, registroDados1->nomeLinha, registroDados1->codProxEstacao, registroDados2->nomeEstacao);

		continua_2:
		free(registroDados2->nomeEstacao); registroDados2->nomeEstacao = NULL;
		free(registroDados2->nomeLinha); registroDados2->nomeLinha = NULL;
		continua_1:
		free(registroDados1->nomeEstacao); registroDados1->nomeEstacao = NULL;
		free(registroDados1->nomeLinha); registroDados1->nomeLinha = NULL;
		continue;
	}

	free(registroDados1);
	free(registroDados2);
	return true;

	erro:
	
	free(registroDados2->nomeEstacao); registroDados2->nomeEstacao = NULL;
	free(registroDados2->nomeLinha); registroDados2->nomeLinha = NULL;
	free(registroDados1->nomeEstacao); registroDados1->nomeEstacao = NULL;
	free(registroDados1->nomeLinha); registroDados1->nomeLinha = NULL;
	free(registroDados1);
	free(registroDados2);
	return false;
}
