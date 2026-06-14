# Documentação Técnica: Índice Árvore-B (B-Tree)

Este diretório contém a implementação completa do **Índice Primário em Árvore-B**, desenhado para indexar registros de dados de forma persistente em disco. A estrutura garante que operações de busca, inserção e remoção ocorram com uma complexidade de tempo logarítmico.

* [Voltar para o README principal](../../README.md)

---

## Especificações 
A árvore possui **ordem $m=4$**, o que dita que cada nó pode conter no máximo 3 chaves e 4 ponteiros.

### 1. Cabeçalho do Arquivo (17 Bytes)
O registo de cabeçalho controla o estado global do arquivo de índice e tem exatamente 17 bytes de tamanho.

| Offset | Tamanho | Campo | Descrição |
| :--- | :--- | :--- | :--- |
| `0` | 1 byte | `status` | `'0'` indica arquivo inconsistente (corrompido ou em uso) e `'1'` indica consistente. |
| `1` | 4 bytes | `noRaiz` | RRN do nó raiz (ou `-1` se a árvore estiver vazia). |
| `5` | 4 bytes | `topo` | RRN do nó no topo da pilha de removidos (ou `-1` se vazia). |
| `9` | 4 bytes | `proxRRN` | Próximo RRN disponível para criação de um novo nó. |
| `13` | 4 bytes | `nroNos` | Número total de nós presentes na Árvore-B. |

### 2. Nó da Árvore-B (53 Bytes)
Cada página (nó) do índice possui 53 bytes, representando uma folha, um nó intermediário ou a raiz.

| Offset | Tamanho | Campo | Descrição |
| :--- | :--- | :--- | :--- |
| `0` | 1 byte | `removido` | `'1'` se o nó estiver logicamente removido, `'0'` caso contrário. |
| `1` | 4 bytes | `proximo` | RRN do próximo nó removido na lista encadeada. |
| `5` | 4 bytes | `tipoNo` | Indica a hierarquia: `-1` (Folha), `0` (Raiz), `1` (Intermediário). |
| `9` | 4 bytes | `nroChaves` | Quantidade atual de chaves no nó.|
| `13` | 4 bytes | `C1`, `C2`, `C3` | Chaves de busca (`codEstacao`). |
| `17` | 4 bytes | `PR1`, `PR2`, `PR3` | *Byte offset* do registo correspondente no arquivo de dados. |
| `29` | 4 bytes | `P1` a `P4` | RRNs dos nós descendentes (`-1` se não existirem). |

---

## Arquitetura de Módulos

A lógica foi dividida para isolar operações de memória, lógica de árvore e acesso a disco:

* `arvoreb_io.c`: Gere a ponte entre a RAM e o disco. Exporta `armazenar_no`, `carregar_no`, `armazenar_cabecalho` e `carregar_cabecalho`.
* `arvoreb_criacao.c`: Lida com a abertura do arquivo (mudando o `status` para inconsistente), criação do cabeçalho nulo e instanciação de novos nós (verificando sempre se a pilha de removidos pode ser reaproveitada).
* `arvoreb_busca.c`: Executa a travessia recursiva (`buscar_chave_rec`). Se a chave corresponder a `C_i`, devolve `PR_i`. Senão, navega para o sub-nó em `P_i`.
* `arvoreb_insercao.c`: Implementa o algoritmo de inserção, que desce recursivamente até à folha adequada. Trata da lógica de *Split* e Promoção.
* `arvoreb_remocao.c`: Coordena as operações de eliminação, incluindo substituição de chaves internas, Empréstimo (Redistribuição) e Fusão (Concatenação).
* `arvoreb_aux.c`: Operações de manipulação interna do nó, como funções para facilitar a ordenação (com o `qsort`) de chaves e ponteiros em memória.

---

## Comportamento Algorítmico

### 1. Inserção (*Split* e Promoção)
A inserção ocorre inicialmente sempre em um nó folha.
Quando um nó atinge a capacidade máxima de 3 chaves e necessitamos inserir uma quarta, ocorre o processo de **Split**.
* **Promoção Mediana:** As 4 chaves são ordenadas em memória RAM. A chave mediana é promovida ao nó pai para manter a árvore balanceada.
* **Divisão da Página:** O nó original mantém as chaves menores à esquerda da mediana. Uma nova página é criada e posicionada à direita para armazenar as chaves maiores.
* **Gestão da Raiz:** Se o nó que sofreu *Split* for a Raiz atual, é gerada uma nova Raiz (o nível da árvore cresce), apontando para o nó antigo (esquerda) e para a página recém-criada (direita).

### 2. Remoção (*Underflow*)
A remoção de chaves implementa as regras mais complexas do balanceamento dinâmico:
* **Remoção Não-Folha:** Se a chave a remover não estiver numa folha, os seus dados são primeiramente trocados pela chave sucessora imediata localizada num nó folha. A remoção ocorre nesse nó folha.
* **Redistribuição (Empréstimo):** Se a remoção em uma folha causar *Underflow* (nó fica com menos chaves que o mínimo exigido), tenta-se o empréstimo. A chave separadora desce do pai para o nó com *underflow*, e uma chave da página adjacente sobe para o pai. Tenta-se primeiro com a adjacente à direita, se esta não tiver chaves sobrando, tenta-se com a adjacente à esquerda.
* **Concatenação (Fusão):** Caso as páginas irmãs estejam no limite mínimo e não possam emprestar, força-se a fusão. A chave separadora do pai desce para unir as chaves dos dois irmãos em um único nó. Funde-se preferencialmente com a página à esquerda; se impossível (caso do nó mais à esquerda), funde-se com a página à direita.
* **Pilha de Removidos:** Após uma fusão, uma das páginas fica inutilizada. Esse nó não é excluído fisicamente, mas marcado com `removido = '1'`. Ele é inserido numa pilha para reaproveitamento: seu campo `proximo` aponta para o atual `topo` salvo no cabeçalho, e `topo` é atualizado para apontar para este RRN recém-liberado.
