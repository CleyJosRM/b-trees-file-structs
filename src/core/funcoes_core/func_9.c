// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "core/func.h"

bool func_9(FILE* arquivoBin, FILE* arquivoIndice, int n){

    REG_DADOS_STRUCT* registroInserir = NULL;

    // Lendo cabeçalho:

    int topo, proxRRN;
    fseek(arquivoBin, 1, SEEK_SET);
    if(fread(&topo, 4, 1, arquivoBin) != 1){
        DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER O TOPO DA PILHA NO CABEÇALHO DO BINÁRIO DE DADOS.\n");
        goto erro;
    }
    if(fread(&proxRRN, 4, 1, arquivoBin) != 1){
        DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER O proxRRN NO CABEÇALHO DO BINÁRIO DE DADOS.\n");
        goto erro;
    }

    // Lendo cada registro do usuário e inserindo:

    for(int i = 0; i < n; i++){

        // Lendo o registro do usuário:

        registroInserir = ler_input_reg();
        if(registroInserir == NULL){
            DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER O %d-ÉSIMO REGISTRO DADO PELO USUÁRIO.\n", i+1);
            continue;
        }

        int rrnInserir;
        long offset;

        // Verificando se um registro com a mesma chave já existe. Se sim, 
        // não se deve inserir um novo registro mas sim substituir o antigo, 
        // pois não podem haver dois registros com mesma chave.

        offset = buscar_entrada(arquivoIndice, registroInserir->codEstacao);
        DEBUG("topo=%d offset=%ld\n", topo, offset);
        if(offset != -1){ // se o registro já estiver no índice, portanto, está no arquivo de dados
    
            DEBUG("O registro com codEstacao %d já existe, está no byteoffset %ld. Substituindo...\n", registroInserir->codEstacao, offset);
            // liberando memória
            if(registroInserir->nomeEstacao) free(registroInserir->nomeEstacao);
            if(registroInserir->nomeLinha) free(registroInserir->nomeLinha);
            free(registroInserir);
            registroInserir = NULL;
            continue;

        // Caso não haja registro com a mesma chave
        // Deve-se verificar se criamos um novo registro com proxRRN ou aproveitamos um removido

        }else if(topo != -1){ // se há registros removidos

            rrnInserir = topo; // o RRN do novo registro é o topo da pilha
            offset = (long)rrnInserir * REG_DADOS_S + HEADER_S; // offset do registro no topo da pilha
            fseek(arquivoBin, offset + 1, SEEK_SET); // apontando para ler o campo "proximo"
            
            int proximo_na_pilha;
            if(fread(&proximo_na_pilha, 4, 1, arquivoBin) != 1){ // lendo o próximo na pilha. Se falhar, libera a memória e sai da função
                DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER QUAL O PRÓXIMO NA PILHA.\n");
                goto erro;
            }
            
            topo = proximo_na_pilha; // atualizando o topo da pilha. Se não há mais registros removidos, vale -1.
        
        // Caso contrário, se não há nenhum registro removido:

        }else{
        
            rrnInserir = proxRRN; // o RRN do novo registro é o proxRRN
            offset = (long)rrnInserir * REG_DADOS_S + HEADER_S; // calculando o offset do registro novinho em folha
        
            proxRRN++; // incrementando proxRRN
        }

        // Escrevendo o registro nos dados e inserindo a entrada cprrespondente no índice

        fseek(arquivoBin, offset, SEEK_SET);

        DEBUG("Escrevendo codEstacao %d no byteoffset %ld\n", registroInserir->codEstacao, offset);

        if(escreve_registro(registroInserir, arquivoBin) == false){
            DEBUG("ERRO EM func_9: NÃO CONSEGUIU ESCREVER O REGISTRO.\n");
            goto erro;
        }

        if(registroInserir->codEstacao != -1){ // codEstacao não pode ser -1 pois esse é o valor usado para indicar "não há chave" na árvore-B
            inserir_entrada(arquivoIndice, registroInserir->codEstacao, offset); // insere uma nova entrada
        }

        // Liberando a memória alocada

        if(registroInserir->nomeEstacao) free(registroInserir->nomeEstacao);
        if(registroInserir->nomeLinha) free(registroInserir->nomeLinha);
        free(registroInserir);
        registroInserir = NULL;
    }

    // Fechando arquivos e retornando:

    atualizar_cabecalho(arquivoBin, topo, proxRRN);

    return true;

    erro:

    if(registroInserir != NULL){
        if(registroInserir->nomeEstacao) free(registroInserir->nomeEstacao);
        if(registroInserir->nomeLinha) free(registroInserir->nomeLinha);
    }
    free(registroInserir);

    return false;
}