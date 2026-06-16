# Sistema de Gerenciamento de Arquivos e Índices (B-Tree)

Este projeto implementa um sistema de gerenciamento de dados em disco baseado em arquivos binários, atuando de forma semelhante a um Sistema de Gerenciamento de Banco de Dados (SGBD) relacional. O projeto usa de um **Índice Primário em Disco utilizando uma Árvore-B**, garantindo alta performance nas operações de busca, inserção e remoção.

---

## O Núcleo do Sistema: Árvore-B (Índice Primário em Disco)

O principal mecanismo de otimização do projeto é uma **Árvore-B de ordem $m=4$**, armazenada integralmente em memória secundária. Em vez de varrer arquivos sequencialmente, o sistema utiliza a árvore para atingir tempo de acesso logarítmico.

Cada nó (página) da árvore ocupa exatamente **53 bytes** no disco. Como as operações manipulam diretamente o armazenamento persistente, toda a navegação requer carregamentos e escritas de forma dinâmica no disco.

## Arquitetura Modular da Árvore-B

### Detalhamento Técnico

Dada a alta complexidade do gerenciamento no disco, envolvendo tratamento de underflows, overflows e navegação, a implementação foi subdividida de forma estruturada no diretório `src/arvoreb/`:

* **`arvoreb_criacao.c`**: Gerencia a criação do arquivo de índice (`.bin`), inicialização do cabeçalho e alocação de novos nós. Controla o reaproveitamento de páginas deletadas usando uma pilha de RRNs.
* **`arvoreb_io.c`**: Concentra e abstrai todo o acesso a disco. Converte as estruturas e arrays de bytes, realizando a interface direta com as rotinas de I/O.
* **`arvoreb_busca.c`**: Implementa os algoritmos de navegação recursiva. Inspeciona chaves internamente nos nós e decide o fluxo da descida ou retorna o *byte offset* do registro no arquivo de dados.
* **`arvoreb_insercao.c`**: Responsável por tentativas de inserção nas folhas. Em casos de página cheia (*overflow*), aciona a lógica de **Split** (divisão do nó 1-para-2) e **Promoção** da chave mediana para o nó pai, propagando até a raiz, se necessário.
* **`arvoreb_remocao.c`**: Gerencia deleções na árvore. Ao detectar número de chaves menor que o mínimo em um nó (*underflow*), tenta realizar o **Empréstimo (Redistribuição)** de páginas irmãs. Caso impossível, executa a **Fusão (Concatenação)** de páginas, propagando o *underflow* recursivamente de baixo para cima.
* **`arvoreb_aux.c` & `debug.c`**: Funções auxiliares matemáticas de *getters/setters*, além de rotinas de impressão para depuração interna de nós.


Para entender a melhor a implementação da nossa estrutura de dados principal, veja:
* [Documentação Técnica da Árvore-B](src/arvoreb/README.md)


---

## Outras Estruturas de Dados Utilizadas

Para operar em conjunto com o índice, o sistema baseia-se em três estruturas secundárias:

1.  **Arquivos Binários**
    O armazenamento principal baseia-se em registros binários de **80 bytes** em disco. Os registros possuem campos de tamanho fixo e variável (indicadores de tamanho para strings). O espaço remanescente não utilizado por strings é preenchido com lixo (`$`), garantindo que o offset seja sempre constante para saltos diretos com operações `fseek`.

2.  **Pilha Encadeada de Reaproveitamento (Remoção Lógica)**
    Quando um registro de dados ou uma página da Árvore-B é deletado, ocorre uma **remoção lógica**. O RRN do item apagado entra no topo de uma lista encadeada controlada pelo cabeçalho do arquivo. Futuras inserções (ou splits da Árvore) resgatam esses RRNs em tempo constante $O(1)$, reduzindo a fragmentação e economizando espaço físico.
    
3.  **Árvore AVL Genérica (Contagem em Memória RAM)**
    A atualização do número de chaves únicas exige processamento. Para evitar a leitura quadrática do arquivo em disco, os dados são temporariamente inseridos em uma Árvore AVL genérica na memória principal durante fechamentos e atualizações. O percurso em-ordem na AVL faz a contagem em tempo e espaço eficientes antes de salvar o cabeçalho final.

A Árvore AVL Genérica foi construída num trabalho anterior do curso de "Algorítmos e Estruturas de Dados". Veja o repositório em:

* [Algorítmos e Estruturas de Dados - Trabalho 2](https://github.com/CleyJosRM/dsa-project2.git)

---

## As 10 Funcionalidades (Interface SQL-like)

A interface de entrada recebe códigos de 1 a 10 que representam rotinas análogas aos principais comandos SQL, implementadas em `core/funcoes_core/`. O roteador principal da aplicação (`src/main.c`) faz o *parsing* dos comandos via linha de comando (`strtok`) e aciona a respectiva rotina:

| Comando | Operação SQL Análoga | Descrição |
| :---: | :--- | :--- |
| **[1]** | `CREATE TABLE` | **Importação CSV:** Lê um `.csv` bruto e constrói o arquivo binário (`.bin`) com registros de 80 bytes. |
| **[2]** | `SELECT * FROM` | **Leitura Sequencial:** Varre o arquivo de dados imprimindo todos os registros válidos, pulando os deletados. |
| **[3]** | `SELECT WHERE` | **Busca Linear com Filtro:** Encontra registros no arquivo binário testando atributos (strings ou inteiros). |
| **[4]** | `DELETE WHERE` | **Deleção Lógica Linear:** Marca o *flag* de remoção como '1' nos registros localizados e os coloca na pilha de reaproveitamento. |
| **[5]** | `INSERT INTO` | **Inserção Direta:** Lê os dados de `stdin` e insere no disco no primeiro *offset* disponível na pilha de removidos (ou no final do arquivo caso a pilha esteja vazia). |
| **[6]** | `UPDATE WHERE` | **Sobrescrita de Registro:** Busca linear para localizar e sobrescrever *in-place* os bytes no disco, preenchendo o restante de novas strings com lixo (`$`). |
| **[7]** | `CREATE INDEX` | **Indexação Árvore-B:** Faz a varredura do arquivo de dados e gera o `.bin` da Árvore-B usando as chaves `codEstacao` como chave primária. |
| **[8]** | `SELECT WHERE` *(Índice)* | **Busca Otimizada:** Identifica se `codEstacao` está nos filtros. Se sim, desce a Árvore-B e resgata o *byte offset* direto do disco em tempo logarítmico. Caso contrário, utiliza a busca sequencial da rotina [3]. |
| **[9]** | `DELETE WHERE` *(Índice)* | **Remoção Otimizada/Sincronizada:** Utiliza a Árvore-B para localizar, deletar logicamente no arquivo de dados, e também realizar o  processo de remoção na respectiva B-Tree. |
| **[10]** | `INSERT INTO` *(Índice)* | **Inserção Sincronizada:** Insere registro (reaproveitando espaços se possível) no dado e já propaga a nova entrada na Árvore-B associada. |

---
