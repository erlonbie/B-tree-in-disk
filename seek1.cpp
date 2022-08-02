#include "csv.h"
#include "functions.h"
#include <cmath>
#include <iostream>

FILE *ponteiroArvore;

/**
 * Busca uma chave na árvore (índice).
 *
 * - Se a busca for bem sucedida, retorna as informações da registro cuja chave corresponde à chave de busca
 *
 * - Caso contrário, retorna uma mensagem informando que ao registro não pode ser encontrado*/
dadoBusca buscaNaArvore(int chave) 
{
  FILE *ponteiroHash = fopen("./hash.bin", "r");
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
        fseek(ponteiroHash, offsetChave * TAMANHO_BLOCO,
              SEEK_SET);
        fread(&bloco, TAMANHO_BLOCO, 1,
              ponteiroHash);
        for (int j = 0; j < bloco.quantidadeArtigos; j++) 
        {
          if (bloco.vetorArtigos[j].ID == chave) {
            dadoBusca result = {bloco.vetorArtigos[j], dadoCabecalho.quantidadeBlocos,
                                dadoCabecalho.alturaArvore};
            printDadosBusca(result);
            fclose(ponteiroHash);
            return result;
          }
        }
      }
    }
  }
  std::cout << "Não encontrou a chave" << std::endl;
  fclose(ponteiroHash);
  return {{}, dadoCabecalho.quantidadeBlocos, dadoCabecalho.alturaArvore};
}

int main(int argc, char *argv[]) 
{

  std::string sEntrada = argv[1];
  int nomeEntrada = stoi(sEntrada);
  ponteiroArvore = fopen("./primarytree.bin", "r");
  buscaNaArvore(nomeEntrada);

  return 0;
}
