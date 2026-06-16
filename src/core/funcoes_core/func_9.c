// Cleyton José Rodrigues Macedo 16821725
// Guilherme Cavalcanti de Santana 15456556

#include "core/definicoes.h"
#include "core/datamanager.h"
#include "arvoreb/indice.h"

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

        if(registroInserir->codEstacao == -1){ // codEstacao não pode ser -1 pois esse é o valor usado para indicar "não há chave" na árvore-B
            goto limpar_memoria;
        }

        // Obtendo o byteoffset do registro a inserir. Isso depende se ele será um novo registro ou um removido reaproveitado.

        bool EhNovoRegistro;
        long offset;
        int proximo_na_pilha;

        if(topo != -1){ // se há registros removidos

            EhNovoRegistro = false;
            // o RRN do novo registro é o topo da pilha
            offset = (long)topo * REG_DADOS_S + HEADER_S; // offset do registro no topo da pilha

            fseek(arquivoBin, offset + 1, SEEK_SET); // apontando para ler o campo "proximo"

            if(fread(&proximo_na_pilha, 4, 1, arquivoBin) != 1){ // lendo o próximo na pilha. Se falhar, libera a memória e sai da função
                DEBUG("ERRO EM func_9: NÃO CONSEGUIU LER QUAL O PRÓXIMO NA PILHA.\n");
                goto erro;
            }

        // Caso contrário, se não há nenhum registro removido:

        }else{
        
            EhNovoRegistro = true;
            // o RRN do novo registro é o proxRRN
            offset = (long)proxRRN * REG_DADOS_S + HEADER_S; // calculando o offset do registro novinho em folha

        }

        // Vamos primeiro tentar inserir a entrada no índice. Se o oposto da
        // função retornar true, é porque já havia uma entrada com a mesma
        // chave, e, portanto, não devemos inserir o registro no registro de
        // dados.

        DEBUG("Escrevendo codEstacao %d no byteoffset %ld\n", registroInserir->codEstacao, offset);

        bool jaExiste = !inserir_entrada(arquivoIndice, registroInserir->codEstacao, offset); // insere uma nova entrada

        if(jaExiste){
            goto limpar_memoria; // não se deve inserir nem atualizar topo e proxRRN. 
        }

        fseek(arquivoBin, offset, SEEK_SET);

        if(escreve_registro(registroInserir, arquivoBin) == false){
            DEBUG("ERRO EM func_9: NÃO CONSEGUIU ESCREVER O REGISTRO.\n");
            goto erro;
        }

        // Atualizando variáveis para o próximo loop

        if(EhNovoRegistro){
            proxRRN++; // incrementando proxRRN
        }else{ // foi um nó removido
            topo = proximo_na_pilha; // atualizando o topo da pilha. Se não há mais registros removidos, vale -1.
        }

        // Liberando a memória alocada
        limpar_memoria:

        if(registroInserir->nomeEstacao) free(registroInserir->nomeEstacao);
        if(registroInserir->nomeLinha) free(registroInserir->nomeLinha);
        free(registroInserir);
        registroInserir = NULL;
    }

    // Fechando arquivos e retornando:

    atualizar_cabecalho(arquivoBin, topo, proxRRN);

    return true;

    erro:
    atualizar_cabecalho(arquivoBin, topo, proxRRN);
    if(registroInserir != NULL){
        if(registroInserir->nomeEstacao) free(registroInserir->nomeEstacao);
        if(registroInserir->nomeLinha) free(registroInserir->nomeLinha);
    }
    free(registroInserir);

    return false;
}