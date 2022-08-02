#include "csv.h"
#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
#include <cstdio>
#include <iostream>

#define QUANTIDADE_PONTEIROS 511
#define QUANTIDADE_BUCKETS 774573
#define TAMANHO_BLOCO 4096

FILE *ponteiroArvore;
std::vector<int> vetorPais;
std::vector<int> vetorPaisFind;

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

int funcaoHash(int chave)
{
  return (chave - 1) % QUANTIDADE_BUCKETS;
}

int contaChaves(noArvore noDado) 
{
  int counter = 0;
  while (noDado.pares[counter].chave != -1 && counter != QUANTIDADE_PONTEIROS - 1)
    counter++;
  return counter;
}

int contaPonteiros(noArvore noDado) 
{
  int counter = 0;
  while (noDado.pares[counter].endereco != -1 && counter != QUANTIDADE_PONTEIROS - 1)
    counter++;
  if (noDado.ponteiroM != -1) 
  {
    counter++;
  }
  return counter;
}

void apagaParesNo(noArvore *no) 
{
  for (int i = 0; i < QUANTIDADE_PONTEIROS - 1; i++) 
  {
    (*no).pares[i].chave = -1;
    (*no).pares[i].endereco = -1;
  }
  (*no).ponteiroM = -1;
}
void copiaParesNo(noArvore *no, noArvoreTemp *tmp, int inicioTemp, int fim) 
{
  for (int i = inicioTemp, j = 0; i <= fim; i++, j++) 
  {
    (*no).pares[j] = (*tmp).pares[i];
  }
}

void copiaTodosParesNo(noArvore *no, noArvoreTemp *tmp) 
{
  for (int i = 0; i < QUANTIDADE_PONTEIROS - 1; i++) 
  {
    (*tmp).pares[i] = (*no).pares[i];
  }
  (*tmp).pares[QUANTIDADE_PONTEIROS - 1].chave = -1;
  (*tmp).pares[QUANTIDADE_PONTEIROS - 1].endereco = -1;
}

void moveParesNo(noArvore *no, int pos, int quntidadeChaves) 
{
  for (int j = quntidadeChaves + 1; j > pos; j--) 
  {
    (*no).pares[j] = (*no).pares[j - 1];
  }
}

void moveParesNo(noArvoreTemp *no, int pos, int quntidadeChaves) 
{
  for (int j = quntidadeChaves; j > pos; j--) 
  {
    (*no).pares[j] = (*no).pares[j - 1];
  }
}

void insereNaFolha(noArvore *no, int chave, int P, int quntidadeChaves) 
{
  int i;
  if (chave < (*no).pares[0].chave) 
  {
    moveParesNo(no, 0, quntidadeChaves);
    (*no).pares[0].chave = chave;
    (*no).pares[0].endereco = P;
  } 
  else 
  {
    if (quntidadeChaves == 0) 
    {
      (*no).pares[i + 1].chave = chave;
      (*no).pares[i + 1].endereco = P;
    } 
    else 
    {
      for (i = quntidadeChaves - 1; i >= 0; i--) 
      {
        if (chave >= (*no).pares[i].chave) 
        {
          moveParesNo(no, i + 1, quntidadeChaves);
          (*no).pares[i + 1].chave = chave;
          (*no).pares[i + 1].endereco = P;
          break;
        }
      }
    }
  }
}

void insereNaFolha(noArvoreTemp *tmp, int chave, int P) 
{
  if (chave < (*tmp).pares[0].chave) 
  {
    moveParesNo(tmp, 0, QUANTIDADE_PONTEIROS - 1);
    (*tmp).pares[0].chave = chave;
    (*tmp).pares[0].endereco = P;
    return;
  }
  for (int i = QUANTIDADE_PONTEIROS - 2; i >= 0; i--) 
  {
    if (chave >= (*tmp).pares[i].chave) 
    {
      moveParesNo(tmp, i + 1, QUANTIDADE_PONTEIROS - 1);
      (*tmp).pares[i + 1].chave = chave;
      (*tmp).pares[i + 1].endereco = P;
      break;
    }
  }
}

int posicaoPai(int node_offset) 
{
  for (int i = 0; i < vetorPais.size(); i++) 
  {
    if (node_offset == vetorPais[i])
    {
      return vetorPais[i - 1];
    }
  }
  return 0;
}

void copiaPaiNo(noArvore *P, noArvoreTempPai *TP, int chave, int offsetChave) 
{
  int i;
  for (i = 0; i < QUANTIDADE_PONTEIROS - 1; i++) 
  {
    (*TP).pares[i] = (*P).pares[i];
  }
  (*TP).pares[i].endereco = (*P).ponteiroM;
  (*TP).pares[i].chave = chave;
  (*TP).ponteiroM = offsetChave;
}

void copiaPorPonteiro(noArvore *no, noArvoreTempPai *TP, int inicioTemp, int fim) 
{
  int i, j;
  for (i = inicioTemp, j = 0; i < fim; i++, j++) 
  {
    (*no).pares[j] = (*TP).pares[i];
  }
  (*no).pares[j].endereco = (*TP).pares[i].endereco;
}

void copiaPorPonteiro2(noArvore *no, noArvoreTempPai *TP, int inicioTemp, int fim) 
{
  int i, j;
  for (i = inicioTemp, j = 0; i < fim; i++, j++) 
  {
    (*no).pares[j] = (*TP).pares[i];
  }
  (*no).pares[j].endereco = (*TP).ponteiroM;
}

void insereNoPai(noArvore *no, int chave, int P, int offsetNo) 
{
  if (vetorPais[0] == offsetNo) 
  {
    noArvore novaRaiz;
    apagaParesNo(&novaRaiz);
    novaRaiz.pares[0].chave = chave;
    novaRaiz.pares[0].endereco = offsetNo;
    novaRaiz.pares[1].endereco = P;

    cabecalhoArvore dadoCabecalho;
    fseek(ponteiroArvore, 0, SEEK_SET);
    fread(&dadoCabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);

    dadoCabecalho.quantidadeBlocos += 1;
    dadoCabecalho.alturaArvore += 1;
    dadoCabecalho.enderecoRaiz = dadoCabecalho.quantidadeBlocos;

    fseek(ponteiroArvore, sizeof(cabecalhoArvore) + TAMANHO_BLOCO * dadoCabecalho.quantidadeBlocos,
          SEEK_SET);
    fwrite(&novaRaiz, sizeof(noArvore), 1, ponteiroArvore);

    fseek(ponteiroArvore, 0, SEEK_SET);
    fwrite(&dadoCabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);
    return;
  }

  int pai = posicaoPai(offsetNo);

  noArvore dadoPai;

  fseek(ponteiroArvore, sizeof(cabecalhoArvore) + pai * TAMANHO_BLOCO, SEEK_SET);
  fread(&dadoPai, sizeof(noArvore), 1, ponteiroArvore);

  int quantidadePonteiro = contaPonteiros(dadoPai);

  if (quantidadePonteiro < QUANTIDADE_PONTEIROS) 
  {
    int i = 0;
    while (dadoPai.pares[i].endereco != offsetNo)
      i++;

    if (i == QUANTIDADE_PONTEIROS - 2) 
    {
      dadoPai.pares[i].chave = chave;
      dadoPai.ponteiroM = P;
    } 
    else 
    {
      dadoPai.pares[i].chave = chave;
      dadoPai.pares[i + 1].endereco = P;
    }
    fseek(ponteiroArvore, -sizeof(noArvore), SEEK_CUR);
    fwrite(&dadoPai, sizeof(noArvore), 1, ponteiroArvore);
  } 
  else 
  {
    noArvoreTempPai TP;
    copiaPaiNo(&dadoPai, &TP, chave, P);
    apagaParesNo(&dadoPai);

    noArvore dadoNovoPai;
    apagaParesNo(&dadoNovoPai);

    copiaPorPonteiro(&dadoPai, &TP, 0, (ceil(QUANTIDADE_PONTEIROS / 2.0) - 1));
    int indicePaiK =
        ceil(QUANTIDADE_PONTEIROS / 2.0) - 1;
    int paiK = TP.pares[indicePaiK].chave;

    fseek(ponteiroArvore, sizeof(cabecalhoArvore) + TAMANHO_BLOCO * pai, SEEK_SET);
    fwrite(&dadoPai, sizeof(noArvore), 1, ponteiroArvore);

    copiaPorPonteiro2(&dadoNovoPai, &TP, (ceil(QUANTIDADE_PONTEIROS / 2.0)),
                       QUANTIDADE_PONTEIROS);

    cabecalhoArvore dadoCabecalho;
    fseek(ponteiroArvore, 0, SEEK_SET);
    fread(&dadoCabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);

    int dadoPosNovoPai = dadoCabecalho.quantidadeBlocos + 1;
    fseek(ponteiroArvore, dadoPosNovoPai * TAMANHO_BLOCO, SEEK_CUR);
    fwrite(&dadoNovoPai, sizeof(noArvore), 1, ponteiroArvore);

    dadoCabecalho.quantidadeBlocos += 1;
    fseek(ponteiroArvore, 0, SEEK_SET);
    fwrite(&dadoCabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);

    insereNoPai(&dadoPai, paiK, dadoPosNovoPai, pai);
  }
}

void insert(int chave, int P) 
{
  vetorPais.clear();
  cabecalhoArvore dadoCabecalho;
  noArvore noDado;
  int noAtual;
  fseek(ponteiroArvore, 0, SEEK_SET);
  fread(&dadoCabecalho, sizeof(cabecalhoArvore), 1,
        ponteiroArvore);
  if (dadoCabecalho.enderecoRaiz == -1)
  {
    noArvore noVazio;
    for (int i = 0; i < QUANTIDADE_PONTEIROS - 1; i++)
    {
      noVazio.pares[i].chave = -1;
      noVazio.pares[i].endereco = -1;
    }
    noVazio.ponteiroM = -1;
    dadoCabecalho.enderecoRaiz = 0;
    dadoCabecalho.alturaArvore = 1;
    fwrite(&noVazio, sizeof(noArvore), 1, ponteiroArvore);
    fseek(ponteiroArvore, 0, SEEK_SET);
    fwrite(&dadoCabecalho, sizeof(cabecalhoArvore), 1,
           ponteiroArvore);
  } 
  else 
  {
    fseek(ponteiroArvore, 0, SEEK_SET);
    fread(&dadoCabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);
    fseek(ponteiroArvore, (dadoCabecalho.enderecoRaiz * TAMANHO_BLOCO),
          SEEK_CUR);

    int nivelAtual = 1;
    noAtual = dadoCabecalho.enderecoRaiz;
    while (nivelAtual !=
           dadoCabecalho.alturaArvore)
    {
      int i = 0;
      fread(&noDado, sizeof(noArvore), 1,
            ponteiroArvore);
      vetorPais.push_back(noAtual);
      while (true) 
      {
        if (i == QUANTIDADE_PONTEIROS - 1 && noDado.pares[i - 1].chave < chave) 
        {
          noAtual = noDado.ponteiroM;
          fseek(ponteiroArvore, sizeof(cabecalhoArvore) + noAtual * TAMANHO_BLOCO,
                SEEK_SET);
          nivelAtual++;
          break;
        } 
        else if (noDado.pares[i].chave > chave ||
                   noDado.pares[i].chave ==
                       -1)
        {
          noAtual = noDado.pares[i].endereco;
          fseek(ponteiroArvore, sizeof(cabecalhoArvore) + noAtual * TAMANHO_BLOCO,
                SEEK_SET);
          nivelAtual++;
          break;
        }
        i++;
      }
    }
  }
  vetorPais.push_back(noAtual);
  fread(&noDado, sizeof(noArvore), 1,
        ponteiroArvore);
  int quntidadeChaves = contaChaves(noDado);
  if (quntidadeChaves < QUANTIDADE_PONTEIROS - 1) 
  {
    insereNaFolha(&noDado, chave, P, quntidadeChaves);
    fseek(ponteiroArvore, -sizeof(noArvore), SEEK_CUR);
    fwrite(&noDado, sizeof(noArvore), 1, ponteiroArvore);
  }
  else
  {
    noArvore novoNoL;
    apagaParesNo(&novoNoL);
    noArvoreTemp tmp;
    copiaTodosParesNo(&noDado, &tmp);
    insereNaFolha(&tmp, chave, P);

    novoNoL.ponteiroM = noDado.ponteiroM;
    int novoNoOffsetL = dadoCabecalho.quantidadeBlocos + 1;
    dadoCabecalho.quantidadeBlocos++;
    noDado.ponteiroM = novoNoOffsetL;
    apagaParesNo(&noDado);
    copiaParesNo(&noDado, &tmp, 0, ceil(QUANTIDADE_PONTEIROS / 2.0) - 1); // talvez voltar pra inteiro
    copiaParesNo(&novoNoL, &tmp, ceil(QUANTIDADE_PONTEIROS / 2.0), QUANTIDADE_PONTEIROS - 1); // talvez voltar pra inteiro
    int menoK = novoNoL.pares[0].chave;
    fseek(ponteiroArvore, -sizeof(noArvore), SEEK_CUR);
    fwrite(&noDado, sizeof(noArvore), 1, ponteiroArvore);
    fseek(ponteiroArvore, sizeof(cabecalhoArvore) + novoNoOffsetL * TAMANHO_BLOCO, SEEK_SET);
    fwrite(&novoNoL, sizeof(noArvore), 1, ponteiroArvore);
    fseek(ponteiroArvore, 0, SEEK_SET);
    fwrite(&dadoCabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);

    insereNoPai(&noDado, menoK, novoNoOffsetL, noAtual);
  }
}

void print_result(dadoBusca d) 
{
  std::cout << "Id: " << d.artigoDado.ID << std::endl;
  std::cout << "Title: " << d.artigoDado.Titulo << std::endl;
  std::cout << "Year : " << d.artigoDado.Ano << std::endl;
  std::cout << "Authors: " << d.artigoDado.Autores << std::endl;
  std::cout << "Citations: " << d.artigoDado.Citacoes << std::endl;
  std::cout << "Update: " << d.artigoDado.Atualizacao << std::endl;
  std::cout << "Snippet: " << d.artigoDado.Snippet << std::endl;
  std::cout << "Blocos lidos: " << d.node_level << std::endl;
  std::cout << "Quantidade de blocos: " << d.quantidadeBlocos + 1 << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
}

dadoBusca find(int chave) 
{
  FILE *hash_file = fopen("../thayna/db-tp2/hash.bin", "r");
  cabecalhoArvore dadoCabecalho;
  noArvore noDado;
  tipoBloco bloco;
  int noAtual;
  fseek(ponteiroArvore, 0, SEEK_SET);
  fread(&dadoCabecalho, sizeof(cabecalhoArvore), 1,
        ponteiroArvore);
  if (dadoCabecalho.enderecoRaiz == -1)
  {
    std::cout << "Error: Ávore vazia!!" << std::endl;
  } 
  else 
  {
    fseek(ponteiroArvore, 0, SEEK_SET);
    fread(&dadoCabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);
    fseek(ponteiroArvore, (dadoCabecalho.enderecoRaiz * TAMANHO_BLOCO),
          SEEK_CUR);

    int nivelAtual = 1;
    noAtual = dadoCabecalho.enderecoRaiz;
    while (nivelAtual !=
           dadoCabecalho.alturaArvore)
    {
      int i = 0;
      fread(&noDado, sizeof(noArvore), 1,
            ponteiroArvore);
      while (true) 
      {
        if (i == QUANTIDADE_PONTEIROS - 1 && noDado.pares[i - 1].chave < chave) 
        {
          noAtual = noDado.ponteiroM;
          fseek(ponteiroArvore, sizeof(cabecalhoArvore) + noAtual * TAMANHO_BLOCO,
                SEEK_SET);
          nivelAtual++;
          break;
        } 
        else if (noDado.pares[i].chave > chave ||
                   noDado.pares[i].chave ==
                       -1)
        {
          noAtual = noDado.pares[i].endereco;
          fseek(ponteiroArvore, sizeof(cabecalhoArvore) + noAtual * TAMANHO_BLOCO,
                SEEK_SET);
          nivelAtual++;
          break;
        }
        i++;
      }
    }
    fread(&noDado, sizeof(noArvore), 1, ponteiroArvore);
    int quntidadeChaves = contaChaves(noDado);
    for (int i = 0; i < quntidadeChaves; i++)
    {
      if (noDado.pares[i].chave == chave)
      {
        int offsetChave = noDado.pares[i].endereco;
        fseek(hash_file, offsetChave * TAMANHO_BLOCO,
              SEEK_SET);
        fread(&bloco, TAMANHO_BLOCO, 1,
              hash_file);
        for (int j = 0; j < bloco.quantidadeArtigos; j++) 
        {
          std::cout << "bloco.id: " << bloco.vetorArtigos[j].ID << std::endl;
          if (bloco.vetorArtigos[j].ID == chave) {
            dadoBusca result = {bloco.vetorArtigos[j], dadoCabecalho.quantidadeBlocos,
                                dadoCabecalho.alturaArvore};
            print_result(result);
            fclose(hash_file);
            return result;
          }
        }
      }
    }
  }
  std::cout << "Não encontrou a chave" << std::endl;
  fclose(hash_file);
  return {{}, dadoCabecalho.quantidadeBlocos, dadoCabecalho.alturaArvore};
}

int main() 
{

  bool leitura = true;
  ponteiroArvore = fopen("./primarytree.bin", "w+"); // coloque w++

  cabecalhoArvore a_cabecalho;
  a_cabecalho.alturaArvore = 0;
  a_cabecalho.quantidadeBlocos = 0;
  a_cabecalho.enderecoRaiz = -1;
  fwrite(&a_cabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);

  io::CSVReader<7, io::trim_chars<>, io::double_quote_escape<';', '\"'>> sample(
      "sample_final.csv");
  sample.set_header("ID", "Titulo", "Ano", "Autores", "Citacoes", "Atualizacao",
                    "Snippet");

  tipoArtigoLeitura *ta_aux =
      (tipoArtigoLeitura *)malloc(sizeof(tipoArtigoLeitura));
  while (leitura) 
  {
    try 
    {
      if (leitura = sample.read_row(ta_aux->ID, ta_aux->Titulo, ta_aux->Ano,
                                    ta_aux->Autores, ta_aux->Citacoes,
                                    ta_aux->Atualizacao, ta_aux->Snippet)) 
      {
        int id = std::stoi(ta_aux->ID);
        insert(id, funcaoHash(id));
        std::cout << "Inserindo id: " << ta_aux->ID << std::endl;
      }
    } 
    catch (io::error::too_few_columns) {}
  }
  free(ta_aux);

  // ponteiroArvore = fopen("./primarytree.bin", "novaRaiz"); //coloque w++
  // find(2);

  fclose(ponteiroArvore);

  return 0;
}
