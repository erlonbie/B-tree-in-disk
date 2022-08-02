#include "csv.h"
#include "functions.h"
#include <cmath>
#include <iostream>

FILE *ponteiroArvore;

/**
 * Essa função insere na folha sempre quando há espaço no nó */
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

/**
 * Função chamada dentro da funcão insere quando a quantidade de chaves não é menor que N-1 */
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

/**
 * Essa funcão é chamada quando não há espaço sufuciente na folha 
 *
 * É feito então um split das folhas, tranferindo a chave a ser inserida para um nó pai.
 *
 * Se esse processo não for sufuciente, a chave a ser inserida é propagada até chegar na raiz da árvore*/
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

/**
 * Função principal que começa o processo de inserção.*/
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
  bool leitura = true;
  std::string nomeEntrada = argv[1];
  ponteiroArvore = fopen("./primarytree.bin", "w+");
  cabecalhoArvore a_cabecalho;
  a_cabecalho.alturaArvore = 0;
  a_cabecalho.quantidadeBlocos = 0;
  a_cabecalho.enderecoRaiz = -1;
  fwrite(&a_cabecalho, sizeof(cabecalhoArvore), 1, ponteiroArvore);
  io::CSVReader<7, io::trim_chars<>, io::double_quote_escape<';', '\"'>> sample(nomeEntrada);
  sample.set_header("ID", "Titulo", "Ano", "Autores", "Citacoes", "Atualizacao",
                    "Snippet");
    FILE *ponteiroHash = fopen("./hash.bin", "r+");
  
    if(ponteiroHash == NULL){
        std::cout << "Error create hash.bin\n";
        return 0; 
    }

    for(int i = 0; i < QUANTIDADE_BUCKETS ; i++){
        tipoBloco bloco;
        fwrite(&bloco, TAMANHO_BLOCO, 1, ponteiroHash);
    }

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
        insere(id, funcaoHash(id));

        tipoArtigo artigo;
        tipoBloco bloco;

        artigo.ID = stoi(ta_aux->ID);
        strcpy(artigo.Titulo, ta_aux->Titulo.c_str());
        artigo.Ano = stoi(ta_aux->Ano);
        strcpy(artigo.Autores, ta_aux->Autores.c_str());
        artigo.Citacoes = stoi(ta_aux->Citacoes);
        strcpy(artigo.Atualizacao, ta_aux->Atualizacao.c_str());
        strcpy(artigo.Snippet, ta_aux->Snippet.c_str());


        fseek(ponteiroHash, funcaoHash(artigo.ID)*TAMANHO_BLOCO, SEEK_SET);
        fread(&bloco, TAMANHO_BLOCO, 1, ponteiroHash);
        bloco.vetorArtigos[bloco.quantidadeArtigos] = artigo;
        bloco.quantidadeArtigos++;
        fseek(ponteiroHash, funcaoHash(artigo.ID)*TAMANHO_BLOCO, SEEK_SET);
        fwrite(&bloco, TAMANHO_BLOCO, 1, ponteiroHash);

        std::cout << "Inserindo id: " << ta_aux->ID << std::endl;
      }
    } 
    catch (io::error::too_few_columns) {}
    catch (io::error::escaped_string_not_closed) {}
  }
  free(ta_aux);
  fclose(ponteiroArvore);
  fclose(ponteiroHash);

  // ponteiroArvore = fopen("./primarytree.bin", "r");
  // buscaNaArvore(500000);

  return 0;
}
