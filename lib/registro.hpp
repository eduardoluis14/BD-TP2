#ifndef H_REGISTRO
#define H_REGISTRO

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>

using namespace std;

// Definição do registro
struct Registro {
    int id;
    string titulo;
    int ano;
    string autores;
    int citacoes;
    string atualizacao;
    string snippet;
    int tamanho;
};

// função para imprimir os dados de um registro
void imprimir_registro(Registro registro) {
    cout << "\tID: " << registro.id << endl;
    cout << "\tTitulo: " << registro.titulo << endl;
    cout << "\tAno: " << registro.ano << endl;
    cout << "\tAutores: " << registro.autores << endl;
    cout << "\tCitacoes: " << registro.citacoes << endl;
    cout << "\tAtualizacao: " << registro.atualizacao << endl;
    cout << "\tSnippet: " << registro.snippet << endl;
    cout << "\tTamanho do registro: " << registro.tamanho << " bytes" << endl;
}

// função para criar um registro
Registro* criar_registro(string id, string titulo, string ano, string autores, string citacoes, string atualizacao, string snippet) {
    Registro* registro = new Registro();
    registro->id = stoi(id);
    registro->titulo = titulo;
    registro->ano = stoi(ano);
    registro->autores = autores;
    registro->citacoes = stoi(citacoes);
    registro->atualizacao = atualizacao;
    registro->snippet = snippet;
    registro->tamanho = registro->titulo.size() + registro->autores.size() + registro->atualizacao.size() + registro->snippet.size() + 4 * sizeof(int) + 4;
    return registro;
}

// função para remover caracteres inválidos (que não sejam printáveis)
string remove_caracteres(string str){
    for (int i = 0; i < str.length(); i++) {
        if (!isprint(str[i])) {
            str[i] = ' '; // substitui por um espaço em branco
        }
    }
    return str;
}

// função para converter uma linha do arquivo de entrada em um registro
Registro* converter_linha_registro(string entry_linha){
    vector<string> registro;
    string campo = "";
    
    string linha = remove_caracteres(entry_linha);

    for (int i = 0; i < linha.size()-1; i++){    
        if (linha[i] == '"'){
            i++;
            while(!(linha[i] == '"' && linha[i+1] == ';') && i < linha.size()-2){
                campo += linha[i];
                i++;
            }
            registro.push_back(campo);
            campo = "";
        } else if (linha[i] == 'N' || (linha[i] == ';' && linha[i+1] == ';')){
            registro.push_back("NULL");
        }
    }

    if (registro.size() != 7){
        return NULL;
    }
    try{
       return criar_registro(registro[0], registro[1], registro[2], registro[3], registro[4], registro[5], registro[6]);
    }
    catch(const std::exception& e){
        return NULL;
    }
}


#endif