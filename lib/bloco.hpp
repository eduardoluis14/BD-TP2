#ifndef H_BLOCO
#define H_BLOCO

#include "registro.hpp"
#define TAM_BLOCO 4096

using namespace std;

// definição do cabeçalho do bloco
struct BlocoCabecalho {
    int num_registros;
    int espaco_livre;
    int pos_registros[18];
};

// definição do bloco
struct Bloco {
    BlocoCabecalho* cabecalho;
    unsigned char dados[TAM_BLOCO];
};

// função para criar um Cabeçalho
BlocoCabecalho* criar_cabecalho() {
    BlocoCabecalho* cabecalho = new BlocoCabecalho();
    cabecalho->num_registros = 0;
    for (int i = 0; i < 18; i++) {
        cabecalho->pos_registros[i] = 0;
    }
    cabecalho->espaco_livre = TAM_BLOCO - sizeof(int) * 2 - sizeof(int) * 18;

    return cabecalho;
}

// função para criar um bloco
Bloco* criar_bloco() {
    Bloco* bloco = new Bloco();
    bloco->cabecalho = criar_cabecalho();
    for(int i = 0; i < TAM_BLOCO; i++) {
        bloco->dados[i] = 0;
    }
    return bloco;
}

// função para desalocar um bloco
void desalocar_bloco(Bloco* bloco) {
    delete bloco->cabecalho;
    delete bloco;
}

#endif