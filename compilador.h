#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <unordered_map>
#include <string>
using namespace std;

// --------- CLASSES DE TOKENS ---------
enum Tag { TYPE = 256, NUM, ID };

struct Token {
    int tag;
    Token() : tag(0) {}
    Token(int t) : tag(t) {}
    virtual string toString() { stringstream ss; ss << char(tag); return ss.str(); }
};

// Estrutura para representar numeros, incluindo ponto-flutuante
struct Num : public Token {
    double value;
    Num() : Token(Tag::NUM), value(0) {}
    Num(double v) : Token(Tag::NUM), value(v) {}
    virtual string toString() { stringstream ss; ss << value; return ss.str(); }
};

struct Id : public Token {
    string name;
    Id() : Token(Tag::ID) {}
    Id(int t, string s) : Token(t), name(s) {}
    virtual string toString() { return name; }
};

// --------- LEXER ---------
class Lexer {
private:
    struct {
        Token t;
        Num n;
        Id i;
    } token;

    int line = 1;
    char peek;
    unordered_map<string, Id> id_table;
    ifstream& input;

public:
    Lexer(ifstream& in) : input(in) {
        id_table["int"] = Id{ Tag::TYPE, "int" };
        id_table["char"] = Id{ Tag::TYPE, "char" };
        id_table["bool"] = Id{ Tag::TYPE, "bool" };
        id_table["float"] = Id{ Tag::TYPE, "float" };
        peek = input.get();
    }

    int Lineno() { return line; }

    Token* Scan() {
        while (isspace(peek)) {
            if (peek == '\n') line++;
            peek = input.get();
        }
        // Reconhecimento de numeros (inteiros e ponto-flutuante)
        if (isdigit(peek)) {
            int v = 0;
            bool is_float = false;
            double float_value = 0.0;
            
            // Leitura da parte inteira
            do {
                int n = peek - '0';
                v = 10 * v + n;
                peek = input.get();
            } while (isdigit(peek));

            // Verifica se ha ponto decimal para identificar ponto-flutuante
            if (peek == '.') {
                is_float = true;
                double divisor = 10.0; // Divisor para a parte fracionaria
                peek = input.get();
                while (isdigit(peek)) {
                    int n = peek - '0';
                    float_value += n / divisor; // Calcula parte fracionaria
                    divisor *= 10;
                    peek = input.get();
                }
                float_value += v; // Soma parte inteira e fracionaria
            }

            // Retorna um token do tipo NUM com o valor correto
            if (is_float) {
                token.n = Num{ float_value };
                return &token.n;
            }

            token.n = Num{ static_cast<double>(v) };
            return &token.n;
        }

        if (isalpha(peek)) {
            stringstream ss;
            do {
                ss << peek;
                peek = input.get();
            } while (isalpha(peek));
            string s = ss.str();
            auto pos = id_table.find(s);
            if (pos != id_table.end()) {
                token.i = pos->second;
                return &token.i;
            }
            Id new_id{ Tag::ID, s };
            id_table[s] = new_id;
            token.i = new_id;
            return &token.i;
        }

        Token op{ peek };
        peek = ' ';
        token.t = op;
        return &token.t;
    }
};

// --------- TABELA DE SÍMBOLOS ---------
struct Symbol {
    string var;
    string type;
};

class SymTable {
private:
    unordered_map<string, Symbol> table;
    SymTable* prev;

public:
    SymTable() : prev(nullptr) {}
    SymTable(SymTable* t) : prev(t) {}

    bool Insert(string s, Symbol symb) {
        auto result = table.insert({ s, symb });
        return result.second;
    }

    Symbol* Find(string s) {
        for (SymTable* st = this; st != nullptr; st = st->prev) {
            auto found = st->table.find(s);
            if (found != st->table.end())
                return &found->second;
        }
        return nullptr;
    }
};

// --------- TRATAMENTO DE ERROS ---------
class SyntaxError {
private:
    int lineno;
    string desc;

public:
    SyntaxError(int line, string msg) : lineno(line), desc(msg) {}

    void What() {
        cout << "\nErro (linha " << lineno << "): " << desc << endl;
    }
};

// --------- PARSER ---------
class Parser {
private:
    Lexer& scanner;
    SymTable* symtable;
    Token* lookahead;

    void Program() {
        Block();
    }

    void Block() {
        if (!Match('{'))
            throw SyntaxError(scanner.Lineno(), "'{' esperado");
        else
            cout << "{ ";

        SymTable* saved = symtable;
        symtable = new SymTable(symtable);

        Decls();
        Stmts();

        if (!Match('}'))
            throw SyntaxError(scanner.Lineno(), "'}' esperado");
        else
            cout << "} ";

        delete symtable;
        symtable = saved;
    }

    void Decls() {
        while (lookahead->tag == Tag::TYPE) {
            string type{ lookahead->toString() };
            Match(Tag::TYPE);

            string name{ lookahead->toString() };
            Match(Tag::ID);

            Symbol s{ name, type };
            if (!symtable->Insert(name, s)) {
                stringstream ss;
                ss << "variavel \"" << name << "\" ja definida";
                throw SyntaxError(scanner.Lineno(), ss.str());
            }

            if (!Match(';')) {
                stringstream ss;
                ss << "encontrado \"" << lookahead->toString() << "\" no lugar de ';'";
                throw SyntaxError(scanner.Lineno(), ss.str());
            }
        }
    }

    void Stmts() {
        while (true) {
            switch (lookahead->tag) {
            case '{':
                Block();
                break;
            case Tag::ID:
            case Tag::NUM:
                Fact();
                if (!Match(';')) {
                    stringstream ss;
                    ss << "encontrado \"" << lookahead->toString() << "\" no lugar de ';'";
                    throw SyntaxError(scanner.Lineno(), ss.str());
                }
                break;
            default:
                return;
            }
        }
    }

    void Fact() {
        if (lookahead->tag == Tag::ID) {
            Symbol* s = symtable->Find(lookahead->toString());
            if (!s) {
                stringstream ss;
                ss << "variavel \"" << lookahead->toString() << "\" nao declarada";
                throw SyntaxError(scanner.Lineno(), ss.str());
            }
            string current_var = s->var;
            cout << current_var << ":" << s->type << "; ";
            Match(Tag::ID);

            if (lookahead->tag == '=') {
                cout << current_var << " = ";
                Match('=');
                if (lookahead->tag == Tag::NUM) {
                    cout << lookahead->toString() << (lookahead->toString().find('.') != string::npos ? ":float; " : ":int; ");
                    Match(Tag::NUM);
                } else {
                    throw SyntaxError(scanner.Lineno(), "valor invalido na atribuicao");
                }
            }
        } else if (lookahead->tag == Tag::NUM) {
            cout << lookahead->toString() << (lookahead->toString().find('.') != string::npos ? ":float; " : ":int; ");
            Match(Tag::NUM);
        } else {
            stringstream ss;
            ss << "\'" << lookahead->toString() << "\' invalido na expressao";
            throw SyntaxError(scanner.Lineno(), ss.str());
        }
    }

    bool Match(int tag) {
        if (tag == lookahead->tag) {
            lookahead = scanner.Scan();
            return true;
        }
        return false;
    }

public:
    Parser(Lexer& lex) : scanner(lex), symtable(nullptr) {
        lookahead = scanner.Scan();
    }

    void Start() {
        Program();
    }
};
