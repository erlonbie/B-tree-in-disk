#include "dados.h"
#include <bits/stdc++.h>

#include <iostream>

std::vector<int> vetorPais;
std::vector<int> vetorPaisFind;

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


void printDadosBusca(dadoBusca d) 
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
