// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

int comparar_codProxEstacao(const void* reg1, const void* reg2){
	REG_DADOS_STRUCT* estacaoEsq = *(REG_DADOS_STRUCT**) reg1;
	REG_DADOS_STRUCT* estacaoDir = *(REG_DADOS_STRUCT**) reg2;

	if (estacaoEsq->codProxEstacao == -1 && estacaoDir->codProxEstacao != -1) return 1;
    if (estacaoEsq->codProxEstacao != -1 && estacaoDir->codProxEstacao == -1) return -1;

	return estacaoEsq->codProxEstacao - estacaoDir->codProxEstacao;
}

int comparar_codEstacao(const void* reg1, const void* reg2){
	REG_DADOS_STRUCT* estacaoEsq = *(REG_DADOS_STRUCT**) reg1;
	REG_DADOS_STRUCT* estacaoDir = *(REG_DADOS_STRUCT**) reg2;

	// Valores nulos devem ficar por último:
	if (estacaoEsq->codEstacao == -1 && estacaoDir->codEstacao != -1) return 1;
    if (estacaoEsq->codEstacao != -1 && estacaoDir->codEstacao == -1) return -1;

	return estacaoEsq->codEstacao - estacaoDir->codEstacao;
}

bool func_13(FILE* arquivoDados1, FILE* arquivoDados2){

	// Lendo informações importantes do cabeçalho do arquivoDados1:

	int nroEstacoes;
	int nroParesEstacao;
	fseek(arquivoDados1, 9, SEEK_SET);
	fread(&nroEstacoes, 4, 1, arquivoDados1);
	fread(&nroParesEstacao, 4, 1, arquivoDados1);
	
	// Criando array de ponteiros para struct registro:
	
	int i=0; // qtd de ponteiros no array
	int tam_array = 32;
    REG_DADOS_STRUCT** registroDados = (REG_DADOS_STRUCT**)malloc(tam_array * sizeof(REG_DADOS_STRUCT*));
   	if(registroDados == NULL){
   		DEBUG("ERRO EM func_13: ALOCAÇÃO DE registroDados FALHOU.\n");
   		goto erro;
   	}

    fseek(arquivoDados1, HEADER_S, SEEK_SET);
    
    while(1) {
        // Aloca a struct para a posição atual
        registroDados[i] = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
        if(registroDados[i] == NULL){
   			DEBUG("ERRO EM func_13: ALOCAÇÃO DE registroDados[%d] FALHOU.\n", i);
   			goto erro;
   		}

        if(load_registro(arquivoDados1, registroDados[i]) == false) {
            free(registroDados[i]); // libera o struct
            break;
        }else if(registroDados[i]->removido == '1'){ // se o registro for logicamente removido, pula
        	free(registroDados[i]->nomeEstacao);
        	free(registroDados[i]->nomeLinha);
        	free(registroDados[i]);
        	continue;
        }
        
        i++;
        if(i == tam_array){
            tam_array *= 2;
            registroDados = (REG_DADOS_STRUCT**)realloc(registroDados, tam_array * sizeof(REG_DADOS_STRUCT*));
        	if(registroDados == NULL){
        		DEBUG("ERRO EM func_13: REALOCAÇÃO DE registroDados FALHOU.\n");
   				goto erro;
        	}
        }
    }

    // usando as funções de comparação de registros para ordenar em memória principal
    qsort(registroDados, i, sizeof(REG_DADOS_STRUCT*), comparar_codEstacao);

    // Escrevendo o cabeçalho no arquivo de saída
    
    unsigned char status = '0';
    int topo = -1; // o novo arquivo não tem pilha de registros logicamente removidos
    int proxRRN = i; // o novo arquivo tem i registros, pois apagamos os logicamente removidos
    fseek(arquivoDados2, 0, SEEK_SET);
    fwrite(&status, 1, 1, arquivoDados2);
    fwrite(&topo, 4, 1, arquivoDados2);
    fwrite(&proxRRN, 4, 1, arquivoDados2);
    fwrite(&nroEstacoes, 4, 1, arquivoDados2);
    fwrite(&nroParesEstacao, 4, 1, arquivoDados2);
    
    // Escrevendo os registros de dados ordenados no arquivo de saída:
    for(int j = 0; j < i; j++){
        if(escreve_registro(registroDados[j], arquivoDados2) == false){
        	DEBUG("ERRO EM func_13: ERRO AO ESCREVER REGISTRO EM ARQUIVO ORDENADO.\n");
        	goto erro;
        } 
    }

    // Liberando a memória:

    for(int j = 0; j < i; j++){
        free(registroDados[j]->nomeEstacao);
        free(registroDados[j]->nomeLinha);
        free(registroDados[j]);
    }
    free(registroDados);

    return true;

    erro:

    for(int j = 0; j < i; j++){
        free(registroDados[j]->nomeEstacao);
        free(registroDados[j]->nomeLinha);
        free(registroDados[j]);
    }
    free(registroDados);

    return false;
}
