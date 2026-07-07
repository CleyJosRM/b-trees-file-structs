// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

bool func_13(FILE* arquivoEntrada, char* campoOrd, FILE* arquivoSaida){

	// Criando array de ponteiros para struct registro:
	
	int i=0; // quantidade atual de registros lidos
	int tam_array = 32; // tamanho inicial de array de ponteiros para registro
    REG_DADOS_STRUCT** registroDados = (REG_DADOS_STRUCT**)malloc(tam_array * sizeof(REG_DADOS_STRUCT*));
    if(registroDados == NULL){
   		DEBUG("ERRO EM func_13: ALOCAÇÃO DE registroDados FALHOU.\n");
   		goto erro;
   	}

    // Escolhendo a função de ordenação dos registros 

    int (*funcao_comparacao)(const void* reg1, const void* reg2);

    if(strcmp(campoOrd, "codEstacao") == 0){
    funcao_comparacao = comparar_registros_por_codEstacao;
    }else if(strcmp(campoOrd, "codProxEstacao") == 0){
        funcao_comparacao = comparar_registros_por_codProxEstacao;
    }else{
        DEBUG("ERRO EM func_13: CAMPO DE ORDENAÇÃO INVÁLIDO")
        goto erro;
    }

    // Trazendo os registros do arquivo de entrada para a memória

    fseek(arquivoEntrada, HEADER_S, SEEK_SET); // primeiro registro
    
    while(1) {
        // Aloca espaço para struct registro, armazena ponteiro no array 
        registroDados[i] = (REG_DADOS_STRUCT*)malloc(sizeof(REG_DADOS_STRUCT));
        if(registroDados[i] == NULL){
   			DEBUG("ERRO EM func_13: ALOCAÇÃO DE registroDados[%d] FALHOU.\n", i);
   			goto erro;
   		}

        if(load_registro(arquivoEntrada, registroDados[i]) == false) { // carregando registro na memória
            // se falhar, é porque o arquivo acabou
            free(registroDados[i]); // libera o último struct, pois não será preenchido
            break;
        }else if(registroDados[i]->removido == '1'){ // se o registro for logicamente removido, pula
        	free(registroDados[i]->nomeEstacao);
        	free(registroDados[i]->nomeLinha);
        	free(registroDados[i]);
        	continue;
        }
        
        i++; // incrementa a qtd de registros lidos
        if(i == tam_array){ // se o array encheu
            tam_array *= 2; // dobra o tamanho e realoca
            REG_DADOS_STRUCT** temp = (REG_DADOS_STRUCT**)realloc(registroDados, tam_array * sizeof(REG_DADOS_STRUCT*));
            if(temp == NULL){ // boa prática usar uma variável temporária com relaloc
        		DEBUG("ERRO EM func_13: REALOCAÇÃO DE registroDados FALHOU.\n");
   				goto erro;
        	}
            registroDados = temp;
        }
    }

    // Usando a função de comparação para ordenar os registros em memória principal

    qsort(registroDados, i, sizeof(REG_DADOS_STRUCT*), funcao_comparacao);
    
    // Escrevendo os registros de dados ordenados no arquivo de saída:

    fseek(arquivoSaida, HEADER_S, SEEK_SET);
    for(int j = 0; j < i; j++){
        if(escreve_registro(registroDados[j], arquivoSaida) == false){
        	DEBUG("ERRO EM func_13: ERRO AO ESCREVER REGISTRO EM ARQUIVO ORDENADO.\n");
        	goto erro;
        } 
    }

    // Escrevendo o cabeçalho no arquivo de saída
    
    atualizar_cabecalho(arquivoSaida, -1, i); // topo = -1, proxRRN = i

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
