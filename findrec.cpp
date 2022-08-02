#include "csv.h"
#include "functions.h"
#include <cmath>
#include <iostream>

int main(int argc, char *argv[]){
    FILE *ponteiroHash = fopen("./hash.bin", "rb");
    std::string sEntrada = argv[1];
    int idPesquisa = stoi(sEntrada);
    
    tipoBloco bloco;
    tipoArtigo artigo;
    bool achou = false;
    
    fseek(ponteiroHash, funcaoHash(idPesquisa)*TAMANHO_BLOCO, SEEK_SET);
    fread(&bloco, sizeof(tipoBloco), 1, ponteiroHash);

    for(int i = 0; i < bloco.quantidadeArtigos; i++)
    {
        if(bloco.vetorArtigos[i].ID == idPesquisa)
        {
            achou = true;
            artigo = bloco.vetorArtigos[i];
            break;
        }
    }

    if(achou)
    {
      std::cout << "Id: " << artigo.ID << std::endl;
      std::cout << "Título: " << artigo.Titulo << std::endl;
      std::cout << "Ano: " << artigo.Ano << std::endl;
      std::cout << "Autores: " << artigo.Autores << std::endl;
      std::cout << "Citações: " << artigo.Citacoes << std::endl;
      std::cout << "Atualização: " << artigo.Atualizacao << std::endl;
      std::cout << "Snippet: " << artigo.Snippet << std::endl;
      std::cout << "Total de blocos lidos: " << 1 << std::endl; // sempre será 1 aqui por ser um hash
      std::cout << "Total de blocos do arquivo: " << QUANTIDADE_BUCKETS << std::endl;
    }
    else
    {
      std::cout << "Registro não encontrado!" << std::endl;
    }
    
    fclose(ponteiroHash);
    return 0;
}
