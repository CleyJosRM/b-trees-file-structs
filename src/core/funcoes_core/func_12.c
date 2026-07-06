// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

bool func_12(FILE* arquivoDados1, FILE* arquivoDados2, FILE* arquivoIndice){

	REG_DADOS_STRUCT* registroDados1 = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
	REG_DADOS_STRUCT* registroDados2 = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));

	fseek(arquivoDados1, HEADER_S, SEEK_SET);

	while (load_registro(arquivoDados1, registroDados1) != false){

		if(registroDados1->removido == '1'){
			free(registroDados1->nomeEstacao); registroDados1->nomeEstacao = NULL;
			free(registroDados1->nomeLinha); registroDados1->nomeLinha = NULL;
			continue;
		}

		long int byteoffset = buscar_entrada(arquivoIndice, registroDados1->codProxEstacao);

		if(byteoffset == -1){
			DEBUG("Não existe estação com codEstacao %d\n", registroDados1->codProxEstacao);
			free(registroDados1->nomeEstacao); registroDados1->nomeEstacao = NULL;
			free(registroDados1->nomeLinha); registroDados1->nomeLinha = NULL;
			continue;
		}

		fseek(arquivoDados2, byteoffset, SEEK_SET);

		if(load_registro(arquivoDados2, registroDados2) == false){
			// liberar memória
			return false;
		}
		printf("%d %s %s %d %s\n", registroDados1->codEstacao, registroDados1->nomeEstacao, registroDados1->nomeLinha, registroDados1->codProxEstacao, registroDados2->nomeLinha);

		free(registroDados1->nomeEstacao); registroDados1->nomeEstacao = NULL;
		free(registroDados1->nomeLinha); registroDados1->nomeLinha = NULL;
		free(registroDados2->nomeEstacao); registroDados2->nomeEstacao = NULL;
		free(registroDados2->nomeLinha); registroDados2->nomeLinha = NULL;
	}

	free(registroDados1);
	free(registroDados2);
	return true;
	
}
