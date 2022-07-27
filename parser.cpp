// #include <cstdlib> -- descomentar se necessário
#include <iostream>
#include <vector>
#include "csv.h"

struct tipo_artigo
{
  std::string ID;
  std::string Titulo;
  std::string Ano;
  std::string Autores;
  std::string Citacoes;
  std::string Atualizacao;
  std::string Snippet;
};

std::vector<tipo_artigo> read_sample()
{
  io::CSVReader<7,io::trim_chars<> , io::double_quote_escape<';','\"'>> sample("sample_final.csv");
  sample.read_header(io::ignore_no_column, "ID","Titulo","Ano","Autores","Citacoes","Atualizacao","Snippet");

  tipo_artigo * ta_aux = (tipo_artigo*) malloc(sizeof(tipo_artigo));

  std::vector<tipo_artigo> ta;
  std::cout << "passei aqui" << std::endl;

  while (sample.read_row(ta_aux->ID, ta_aux->Titulo,ta_aux->Ano, ta_aux->Autores, ta_aux->Citacoes, ta_aux->Atualizacao, ta_aux->Snippet)) 
  {
      ta.push_back({ta_aux->ID, ta_aux->Titulo,ta_aux->Ano, ta_aux->Autores, ta_aux->Citacoes, ta_aux->Atualizacao, ta_aux->Snippet});
  }

  free(ta_aux);

  return ta;
}

void display_sample(std::vector<tipo_artigo> ta)
{
  for(auto a : ta)
  {
   std::cout << "ID: " << a.ID << std::endl; 
   std::cout << "Titulo: " << a.Titulo << std::endl; 
   std::cout << "Ano: " << a.Ano << std::endl; 
   std::cout << "Autores: " << a.Autores << std::endl; 
   std::cout << "Citacoes: " << a.Citacoes << std::endl; 
   std::cout << "Atualizacao: " << a.Atualizacao << std::endl; 
   std::cout << "Snippet: " << a.Snippet << std::endl; 
   std::cout << "------------------------" << std::endl; 
  }
}

int main (int argc, char *argv[])
{
  std::vector<tipo_artigo> ta = read_sample();
  display_sample(ta);
  return 0;
}
