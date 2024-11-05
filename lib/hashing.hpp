#ifndef H_HASHING
#define H_HASHING

#include "bucket.hpp"
#define NUM_BUCKETS 15000

using namespace std;

// função que cria a tabela hash e escreve em disco
void init(ofstream& dataFile) {
    for (int i = 0; i < NUM_BUCKETS; i++) {
        criarBucket(dataFile);
    }
}

//função para calcular o hash
int funcao_hash(int id){
    int index = (67 * id) % NUM_BUCKETS;
    return index;
}

// função para inserir um registro em um bloco
void inserir_registro_bloco(ifstream& leitura, ofstream& escrita, Bloco* bloco, Registro* registro, int ultimo_bloco, int index_bucket) {

    int posicao = bloco->cabecalho->pos_registros[bloco->cabecalho->num_registros];
    memcpy(&bloco->dados[posicao], &registro->id, sizeof(int));
    posicao += sizeof(int);
    memcpy(&bloco->dados[posicao], registro->titulo.c_str(), registro->titulo.size() + 1);
    posicao += registro->titulo.size() + 1;
    memcpy(&bloco->dados[posicao], &registro->ano, sizeof(int));
    posicao += sizeof(int);
    memcpy(&bloco->dados[posicao], registro->autores.c_str(), registro->autores.size() + 1);
    posicao += registro->autores.size() + 1;
    memcpy(&bloco->dados[posicao], &registro->citacoes, sizeof(int));
    posicao += sizeof(int);
    memcpy(&bloco->dados[posicao], registro->atualizacao.c_str(), registro->atualizacao.size() + 1);
    posicao += registro->atualizacao.size() + 1;
    memcpy(&bloco->dados[posicao], registro->snippet.c_str(), registro->snippet.size() + 1);
    posicao += registro->snippet.size() + 1;

    bloco->cabecalho->num_registros++;
    bloco->cabecalho->espaco_livre -= registro->tamanho;
    bloco->cabecalho->pos_registros[bloco->cabecalho->num_registros] = posicao;

    char buffer[TAMANHO_BLOCO];
    memcpy(buffer, bloco->cabecalho, sizeof(BlocoCabecalho));
    memcpy(buffer + sizeof(BlocoCabecalho), bloco->dados, TAMANHO_BLOCO - sizeof(BlocoCabecalho));

    escrita.seekp(index_bucket * TAMANHO_BLOCO * NUMERO_BLOCOS + (ultimo_bloco * TAMANHO_BLOCO));
    escrita.write(reinterpret_cast<char*>(buffer), TAMANHO_BLOCO);
}

// função para inserir um registro em um bucket e retornar o endereço do registro no arquivo de dados
int inserir_registro_bucket(Registro *registro, ifstream &entrada, ofstream &saida)
{   
    int indice_bucket = funcao_hash(registro->id);
    int ultimo_bloco = 0; 
    int inicio_bucket = indice_bucket * TAMANHO_BLOCO * NUMERO_BLOCOS; 
    entrada.seekg(inicio_bucket);
    
    for (int i = 0; i < NUMERO_BLOCOS; i++)
    {
        Bloco* bloco = new Bloco();
        bloco->cabecalho = new BlocoCabecalho();
        entrada.read(reinterpret_cast<char*>(bloco->cabecalho), sizeof(BlocoCabecalho));
        entrada.read(reinterpret_cast<char*>(bloco->dados), TAMANHO_BLOCO - sizeof(BlocoCabecalho));
    
        int tam = bloco->cabecalho->espaco_livre;
        if (tam >= registro->tamanho)
        {   
            int addr =  inicio_bucket;
            addr += (ultimo_bloco * TAMANHO_BLOCO) + sizeof(BlocoCabecalho) + bloco->cabecalho->pos_registros[bloco->cabecalho->num_registros];

            inserir_registro_bloco(entrada, saida, bloco, registro, ultimo_bloco, indice_bucket);
            desalocar_bloco(bloco); 
            return addr;
        }else{
            ultimo_bloco++;
            desalocar_bloco(bloco);
        }
        if(i + 1 >= NUMERO_BLOCOS){
            cout << "Erro: Não há espaço disponível para inserir o registro" << endl;
            cout << "Registros inseridos: " << registro->id -1 << endl;
            cout << "Não foi possivel gerar os arquivos de indice" << endl;
            exit(1);
        }
    }
    return -1;
}

//função para buscar um registro no arquivo de dados
Registro* buscar_registro(ifstream& leitura, int id_busca) {
    for (int ultimo_bloco = 0; ultimo_bloco < NUMERO_BLOCOS; ultimo_bloco++) {
        Bloco* bloco = criar_bloco();
        int index_bucket = funcao_hash(id_busca);
        leitura.seekg(index_bucket * TAMANHO_BLOCO * NUMERO_BLOCOS + (ultimo_bloco * TAMANHO_BLOCO));
        leitura.read(reinterpret_cast<char*>(bloco->cabecalho), sizeof(BlocoCabecalho));
        leitura.read(reinterpret_cast<char*>(bloco->dados), TAMANHO_BLOCO - sizeof(BlocoCabecalho));

        if (bloco->cabecalho->num_registros > 0) {
            Registro* registro = new Registro();
            for (int i = 0; i < bloco->cabecalho->num_registros; i++) {
                int posicao = bloco->cabecalho->pos_registros[i];

                memcpy(&registro->id, &bloco->dados[posicao], sizeof(int));
                if(registro->id == id_busca) {
                    posicao += sizeof(int);
                    registro->titulo = string((char *)&bloco->dados[posicao]);
                    posicao += registro->titulo.size() + 1;
                    memcpy(&registro->ano, &bloco->dados[posicao], 2);
                    posicao += sizeof(int);
                    registro->autores = string((char *)&bloco->dados[posicao]);
                    posicao += registro->autores.size() + 1;
                    memcpy(&registro->citacoes, &bloco->dados[posicao], 1);
                    posicao += sizeof(int);
                    registro->atualizacao = string((char *)&bloco->dados[posicao]);
                    posicao += registro->atualizacao.size() + 1;
                    registro->snippet = string((char *)&bloco->dados[posicao]);
                    posicao += registro->snippet.size() + 1;
                    registro->tamanho = registro->titulo.size() + sizeof(int) + registro->autores.size() + sizeof(int) + sizeof(int) + registro->atualizacao.size() + registro->snippet.size() + 4;
                    cout << "\nQuantidade de blocos lidos para encontrar o registro: " << ultimo_bloco + 1 << endl;
                    cout << "Total de blocos no arquivo de dados: " << NUMERO_BLOCOS * NUM_BUCKETS << endl;

                    desalocar_bloco(bloco);

                    return registro;
                }
            }
        }
        desalocar_bloco(bloco);
    }

    return nullptr;
}

#endif