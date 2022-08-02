#define QUANTIDADE_PONTEIROS 511
#define QUANTIDADE_BUCKETS 774573
#define TAMANHO_BLOCO 4096

#include <iostream>

struct tipoArtigoLeitura
{
  std::string ID;
  std::string Titulo;
  std::string Ano;
  std::string Autores;
  std::string Citacoes;
  std::string Atualizacao;
  std::string Snippet;
};

// struct tipoArtigo 
// {
//   int ID = 0;
//   char Titulo[300] = {};
//   int Ano = 0;
//   char Autores[150] = {};
//   int Citacoes = {};
//   char Atualizacao[20] = {};
//   char Snippet[1024] = {};
// };

struct tipoArtigo 
{
  int ID;
  char Titulo[300];
  int Ano;
  char Autores[150];
  int Citacoes;
  char Atualizacao[20];
  char Snippet[1024];
};

struct tipoBloco 
{
  int quantidadeArtigos = 0;
  tipoArtigo vetorArtigos[2] = {};
};

struct dadoBusca
{
  tipoArtigo artigoDado;
  int quantidadeBlocos;
  int node_level;
};

struct cabecalhoArvore 
{
  int enderecoRaiz;
  int quantidadeBlocos;
  int alturaArvore;
};

struct parNo
{
  int endereco;
  int chave;
};

struct noArvore 
{
  parNo pares[QUANTIDADE_PONTEIROS - 1];
  int ponteiroM;
};

struct noArvoreTemp 
{
  parNo pares[QUANTIDADE_PONTEIROS];
};

struct noArvoreTempPai 
{
  parNo pares[QUANTIDADE_PONTEIROS];
  int ponteiroM;
};

