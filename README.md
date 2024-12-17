
# Compilador Simples

Este projeto implementa um compilador simples que realiza a análise léxica e sintática de arquivos de entrada contendo declarações e atribuições de variáveis. O programa identifica erros de sintaxe e variáveis não declaradas, emitindo mensagens apropriadas.

## Arquivos do Projeto

- compilador.h: Contém a definição das classes principais utilizadas no compilador, incluindo:
    - Token e suas subclasses (Num, Id) para representar tokens identificados.
    - Lexer: Responsável pela análise léxica do arquivo de entrada.
    - SymTable: Implementa a tabela de símbolos para armazenar variáveis e tipos.
    - SyntaxError: Gerencia o tratamento de erros sintáticos.
    -  Parser: Realiza a análise sintática e organiza a estrutura de código.
- compilador.cpp: Arquivo principal, que inicializa o Lexer e o Parser e executa o programa.
- testeError.txt: Arquivo de teste com erro de sintaxe para demonstrar o tratamento de erros.

## Como Executar
**Pré-requisitos**

    Compilador g++ (GCC) instalado no sistema.

**Compilação**

- Para compilar o projeto, execute o seguinte comando no terminal:

      g++ compilador.cpp -o compilador

- **Execução**

    - Após a compilação, o programa gerará um arquivo executável chamado compilador.
    - Coloque o arquivo de teste (por exemplo, testeError.txt) na mesma pasta onde o executável foi gerado.
    - Execute o programa com o seguinte comando:

          ./compilador <arquivo_de_entrada>

**Exemplo:**

    ./compilador testeError.txt

#### *OBS: O arquivo de teste tem que fica na mesma pasta do programa exe gerado pela compilação do main.*

## Funcionalidades

  - Análise Léxica: Identificação de tokens (números, identificadores, palavras-chave, etc.).
  - Análise Sintática: Validação da estrutura do código, incluindo:
  - Blocos de código.
  - Declaração de variáveis.
  - Atribuições válidas.
  - Tabela de Símbolos: Armazena variáveis declaradas e seus tipos.
  - Tratamento de Erros: Identifica erros como:
    - Variáveis não declaradas.
    - Uso incorreto de tipos.
    - Tokens inválidos.
    - Falta de delimitadores ou ponto e vírgula.
