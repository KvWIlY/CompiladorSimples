#include <iostream>
#include <fstream>
#include "compilador.h"
using namespace std;

// Arquivo de entrada
int main(int argc, char **argv) {
    if (argc == 2) {
        ifstream fin(argv[1]);
        if (!fin.is_open()) {
            cout << "Falha ao abrir o arquivo \"" << argv[1] << "\".\n";
            cout << "Verifique se o nome está correto ou se o arquivo não é protegido para leitura.\n";
            return EXIT_FAILURE;
        }

        Lexer lexer(fin);         // Inicializa o Lexer com o arquivo de entrada
        Parser tradutor(lexer);   // Inicializa o Parser com o Lexer

        try {
            tradutor.Start();
        } catch (SyntaxError& err) {
            err.What();
        }

        cout << endl;
        fin.close();
    } else {
        cout << "Uso: " << argv[0] << " <arquivo_fonte>\n";
    }

    return 0;
}