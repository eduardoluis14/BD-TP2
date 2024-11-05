#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "bloco.hpp"
#define NUM_BLOCOS 12

using namespace std;

// definição da estrutura do bucket
struct Bucket {
    int qt_blocos;
    int ultimo_bloco;
    Bloco* blocos[NUM_BLOCOS];
};

// função para criar um bucket e blocos temporários e escrever no arquivo de dados
void criarBucket(ofstream &dataFile) {
    Bucket* bucket = new Bucket();
    bucket->qt_blocos = 0;
    bucket->ultimo_bloco = 0;
    for (int i = 0; i < NUM_BLOCOS; i++) {
        bucket->blocos[i] = criar_bloco();
        bucket->qt_blocos++;
        char buffer[TAMANHO_BLOCO];
        memcpy(buffer, bucket->blocos[i]->cabecalho, sizeof(BlocoCabecalho));
        memcpy(buffer + sizeof(BlocoCabecalho), bucket->blocos[i]->dados, TAMANHO_BLOCO - sizeof(BlocoCabecalho));
        dataFile.write(buffer, TAMANHO_BLOCO);
    }
    desalocar_bucket(bucket);
}

// função para desalocar um bucket
void desalocar_bucket(Bucket* bucket) {
    for (int i = 0; i < NUM_BLOCOS; i++) {
        desalocar_bloco(bucket->blocos[i]);
    }
    delete bucket; 
}

#endif