#include "dados.h"
#include <bits/stdc++.h>

#include <iostream>

std::vector<int> vetorPais;
std::vector<int> vetorPaisFind;

/**
 * Cria um hash para uma determinada chave
 *
 * A implementação foi a mais simples possível com uma função mod %*/
int funcaoHash(int chave)
{
  return (chave - 1) % QUANTIDADE_BUCKETS;
}

/**
 * Retorna a quantidade de chaves existentes em um nó **/
int contaChaves(noArvore noDado) 
{
  int counter = 0;
  while (noDado.pares[counter].chave != -1 && counter != QUANTIDADE_PONTEIROS - 1)
    counter++;
  return counter;
}

/**
 * Retorna a quantidade de ponteiros existente em um nó **/
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

/**
 * Apaga os valores de chave e ponteiro, setando para -1 **/
void apagaParesNo(noArvore *no) 
{
  for (int i = 0; i < QUANTIDADE_PONTEIROS - 1; i++) 
  {
    (*no).pares[i].chave = -1;
    (*no).pares[i].endereco = -1;
  }
  (*no).ponteiroM = -1;
}

/**
 * Copia os valores de um nó auxiliar (noArvoreTemp) para o nó noArvore de um determinada posição a outra **/
void copiaParesNo(noArvore *no, noArvoreTemp *tmp, int inicioTemp, int fim) 
{
  for (int i = inicioTemp, j = 0; i <= fim; i++, j++) 
  {
    (*no).pares[j] = (*tmp).pares[i];
  }
}

/**
 * Copia todos os valores de noArvore para uma noArvoreTemp **/
void copiaTodosParesNo(noArvore *no, noArvoreTemp *tmp) 
{
  for (int i = 0; i < QUANTIDADE_PONTEIROS - 1; i++) 
  {
    (*tmp).pares[i] = (*no).pares[i];
  }
  (*tmp).pares[QUANTIDADE_PONTEIROS - 1].chave = -1;
  (*tmp).pares[QUANTIDADE_PONTEIROS - 1].endereco = -1;
}

/**
 * Desloca os valores de um determinado nó para a inserção de uma nova chave **/
void moveParesNo(noArvore *no, int pos, int quntidadeChaves) 
{
  for (int j = quntidadeChaves + 1; j > pos; j--) 
  {
    (*no).pares[j] = (*no).pares[j - 1];
  }
}

/**
 * Desloca os valores de um determinado nó para a inserção de uma nova chave **/
void moveParesNo(noArvoreTemp *no, int pos, int quntidadeChaves) 
{
  for (int j = quntidadeChaves; j > pos; j--) 
  {
    (*no).pares[j] = (*no).pares[j - 1];
  }
}

/**
 * Retorna a posição do pai de um determinado nó em relação ao caminho feito na inserção ou busca */
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

/**
 * Copia todos os ponteiros e chavesde um nó noArvore para um nó noArvoreTempPai, e aadiciona chave e offsetChave no final  */
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

/**
 * Copia os valores de um nó noArvoreTempPai para um nó noArvore, de 0 à teto(N/2)-1  */
void copiaPorPonteiro(noArvore *no, noArvoreTempPai *TP, int inicioTemp, int fim) 
{
  int i, j;
  for (i = inicioTemp, j = 0; i < fim; i++, j++) 
  {
    (*no).pares[j] = (*TP).pares[i];
  }
  (*no).pares[j].endereco = (*TP).pares[i].endereco;
}

/**
 * Copia os valores de um nó noArvoreTempPai para um nó noArvore, de teto(N/2) à até o fim(N)  */
void copiaPorPonteiro2(noArvore *no, noArvoreTempPai *TP, int inicioTemp, int fim) 
{
  int i, j;
  for (i = inicioTemp, j = 0; i < fim; i++, j++) 
  {
    (*no).pares[j] = (*TP).pares[i];
  }
  (*no).pares[j].endereco = (*TP).ponteiroM;
}


/**
 * Imprime os dados de um bloco no arquivo de hash se a consulta for bem sucedida */
void printDadosBusca(dadoBusca d) 
{
  std::cout << "ID: " << d.artigoDado.ID << std::endl;
  std::cout << "Titulo: " << d.artigoDado.Titulo << std::endl;
  std::cout << "Ano : " << d.artigoDado.Ano << std::endl;
  std::cout << "Autores: " << d.artigoDado.Autores << std::endl;
  std::cout << "Citacoes: " << d.artigoDado.Citacoes << std::endl;
  std::cout << "Atualizacao: " << d.artigoDado.Atualizacao << std::endl;
  std::cout << "Snippet: " << d.artigoDado.Snippet << std::endl;
  std::cout << "Blocos lidos: " << d.node_level << std::endl;
  std::cout << "Quantidade de blocos: " << d.quantidadeBlocos + 1 << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
}
