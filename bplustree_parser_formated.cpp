#include "csv.h"
#include "functions.h"
#include <cmath>
#include <iostream>

FILE *ponteiroArvore;

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

void insere(int chave, int P) 
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

dadoBusca buscaNaArvore(int chave) 
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
            printDadosBusca(result);
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
  // bool leitura = true;
  // ponteiroArvore = fopen("./primarytree.bin", "w+");
  // cabecalhoArvore a_cabecalho;
  // a_cabecalho.alturaArvore = 0;
  // a_cabecalho.quantidadeBlocos = 0;
  // a_cabecalho.enderecoRaiz = -1;
  // fwrite(&a_cabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);
  //
  // io::CSVReader<7, io::trim_chars<>, io::double_quote_escape<';', '\"'>> sample(
  //     "./artigo.csv");
  // sample.set_header("ID", "Titulo", "Ano", "Autores", "Citacoes", "Atualizacao",
  //                   "Snippet");
  //
  // tipoArtigoLeitura *ta_aux =
  //     (tipoArtigoLeitura *)malloc(sizeof(tipoArtigoLeitura));
  // while (leitura) 
  // {
  //   try 
  //   {
  //     if (leitura = sample.read_row(ta_aux->ID, ta_aux->Titulo, ta_aux->Ano,
  //                                   ta_aux->Autores, ta_aux->Citacoes,
  //                                   ta_aux->Atualizacao, ta_aux->Snippet)) 
  //     {
  //       int id = std::stoi(ta_aux->ID);
  //       insere(id, funcaoHash(id));
  //       std::cout << "Inserindo id: " << ta_aux->ID << std::endl;
  //     }
  //   } 
  //   catch (io::error::too_few_columns) {}
  //   catch (io::error::escaped_string_not_closed) {}
  // }
  // free(ta_aux);
  // fclose(ponteiroArvore);

  ponteiroArvore = fopen("./primarytree.bin", "r");
  buscaNaArvore(500000);

  return 0;
}
