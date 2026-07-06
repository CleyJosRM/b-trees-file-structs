// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

bool func_11(FILE* arquivoDados1, FILE* arquivoDados2){

	REG_DADOS_STRUCT* registroDados1 = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
	REG_DADOS_STRUCT* registroDados2 = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));

	fseek(arquivoDados1, HEADER_S, SEEK_SET);

	while (load_registro(arquivoDados1, registroDados1) != false){

		if(registroDados1->removido == '1'){
			goto continua_1;
		}
		
		fseek(arquivoDados2, HEADER_S, SEEK_SET);
		// printf("REGISTRO DADOS 1: %d %s\n", registroDados1->codEstacao, registroDados1->nomeEstacao);
		
		while (load_registro(arquivoDados2, registroDados2) != false){
			
			if(registroDados2->removido == '1'){
				goto continua_2;
			}
			// printf("REGISTRO DADOS 2: %d %s\n", registroDados2->codEstacao, registroDados2->nomeEstacao);
			if (registroDados1->codProxEstacao == registroDados2->codEstacao){
				printf("%d %s %s %d %s\n", registroDados1->codEstacao, registroDados1->nomeEstacao, registroDados1->nomeLinha, registroDados1->codProxEstacao, registroDados2->nomeLinha);	
			}

			continua_2:

			free(registroDados2->nomeEstacao);
			registroDados2->nomeEstacao = NULL;
			free(registroDados2->nomeLinha);
			registroDados2->nomeLinha = NULL;
		}

		continua_1:
		
		free(registroDados1->nomeEstacao);
		registroDados1->nomeEstacao = NULL;
		free(registroDados1->nomeLinha);
		registroDados1->nomeLinha = NULL;
	}

	free(registroDados1);
	free(registroDados2);
	return true;

	/**Objetivo: extrair um registro de dados do disco e colocar na memória
	 * 
	 * Pré-condições:
	 *      Filestream aberta em modo que permita leitura
	 *      Cursor posicionada no começo de um registro de arquivos
	 *      Struct mem_destino alocado propriamente
	 * 
	 * Pós-condições:
	 *      Erro: retorna false
	 *      Sucesso: retorna true. O cursor aponta para o próximo registro de dados
	 *      Chamador deve: apagar o registro da memória quando terminar de usar, fechar a filestream com fecha_binario
	 **/
	// bool load_registro(FILE* filestream_bin, REG_DADOS_STRUCT* mem_destino);
}
