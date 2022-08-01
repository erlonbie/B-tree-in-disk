#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include "csv.h"

#define N 511
#define NUM_BUCKETS 774573
#define BLOCK_LENGTH 4096

FILE *tree_file;
std::vector<int> parents;
std::vector<int> controle_parental;

struct tipo_artigo_leitura {
    std::string ID;
    std::string Titulo;
    std::string Ano;
    std::string Autores;
    std::string Citacoes;
    std::string Atualizacao;
    std::string Snippet;
};

struct tipo_artigo {
    int ID = 0;
    char Titulo[300] = {};
    int Ano = 0;
    char Autores[150] = {};
    int Citacoes = {};
    char Atualizacao[20] = {};
    char Snippet[1024] = {};
};

struct block_t {
    int articles_quantity = 0;
    tipo_artigo articles[2] = {};
};

struct find_data{
  tipo_artigo art_data;
  int node_count;
  int node_level;
};

struct head{
    int root_offset;
    int node_count;
    int level_count;
};

struct combo{
    int offset;
    int key;
};

struct node{
    combo pairs[N-1];
    int pointer;
};

struct nodeT{
    combo pairs[N];
};

struct nodeTP{
    combo pairs[N];
    int pointer;
};

int hashify(int key){
    return (key-1) % NUM_BUCKETS;
}

int count_keys(node node_data)
{
    int counter = 0;
    while(node_data.pairs[counter].key != -1 && counter != N-1) counter++;
    return counter;
}

int count_pointers(node node_data)
{
    int counter = 0;
    while(node_data.pairs[counter].offset != -1 && counter != N-1) counter++;
    if (node_data.pointer != -1) 
    {
      counter++;
    }
    return counter;
}

void erase_pairs(node* L)
{
    for(int i = 0; i < N-1; i++)
    {
        (*L).pairs[i].key = -1;
        (*L).pairs[i].offset = -1;
    }
    (*L).pointer = -1;
}
void copy_pairs(node* L, nodeT* T, int beginT, int end)
{
    for(int i = beginT, j = 0; i <= end; i++, j++){
        (*L).pairs[j] = (*T).pairs[i];
    }
}

void copy_all_pairs(node *L, nodeT* T)
{
    for(int i = 0; i < N-1; i++){
        (*T).pairs[i] = (*L).pairs[i];
    }
    (*T).pairs[N-1].key = -1;    
    (*T).pairs[N-1].offset = -1;    
}

void move_pairs(node* L, int i, int keys_quantity)
{
    for(int j = keys_quantity+1; j > i; j--)
    {
        (*L).pairs[j] = (*L).pairs[j-1];
    }
}

void move_pairs(nodeT* L, int i, int keys_quantity)
{
    for(int j = keys_quantity; j > i; j--)
    {
        (*L).pairs[j] = (*L).pairs[j-1];
    }
}

void insert_in_leaf(node* L, int K, int P, int keys_quantity)
{
    int i;
    if(K < (*L).pairs[0].key)
    {
        move_pairs(L, 0, keys_quantity);
        (*L).pairs[0].key = K;
        (*L).pairs[0].offset = P;
    }
    else {
      if (keys_quantity == 0)
      {
          (*L).pairs[i+1].key = K;
          (*L).pairs[i+1].offset = P;
      }
      else 
      {
        for(i = keys_quantity-1; i >= 0; i--)
        {
            if(K >= (*L).pairs[i].key)
            {
                move_pairs(L, i+1, keys_quantity);
                (*L).pairs[i+1].key = K;
                (*L).pairs[i+1].offset = P;
                break;
            }
        }
      }
    }
}

void insert_in_leaf(nodeT* T, int K, int P)
{
    if(K < (*T).pairs[0].key)
    {
        move_pairs(T, 0, N-1);
        (*T).pairs[0].key = K;
        (*T).pairs[0].offset = P;
        return;
    }
    for(int i = N-2; i >= 0; i--)
    {
        if(K >= (*T).pairs[i].key)
        {
            move_pairs(T, i+1, N-1);
            (*T).pairs[i+1].key = K;
            (*T).pairs[i+1].offset = P;
            break;
        }
    }
}

int parent_position(int node_offset)
{
  for (int i = 0; i < parents.size(); i++)
  {
      if (node_offset == parents[i]) 
      {
        return parents[i-1];
      }
  }
  return 0;
}

void copy_parent(node * P, nodeTP *TP, int K, int K_offset)
{
  int i;
  for (i = 0; i < N-1; i++) 
  {
    (*TP).pairs[i] = (*P).pairs[i];
  }
  (*TP).pairs[i].offset = (*P).pointer;
  (*TP).pairs[i].key = K;
  (*TP).pointer = K_offset;
}

void copy_per_pointers(node* L, nodeTP* TP, int beginT, int end)
{
  int i, j;
  for (i = beginT, j = 0; i < end; i++, j++)
  {
    (*L).pairs[j] = (*TP).pairs[i];
  }
  (*L).pairs[j].offset = (*TP).pairs[i].offset;
}

void copy_per_pointers2(node* L, nodeTP* TP, int beginT, int end)
{
  int i, j;
  for (i = beginT, j = 0; i < end; i++, j++)
  {
    (*L).pairs[j] = (*TP).pairs[i];
  }
  (*L).pairs[j].offset = (*TP).pointer;
}

void insert_in_parent(node * L, int K, int P, int L_offset)
{
  if (parents[0] == L_offset) 
  {
    node R;
    erase_pairs(&R);
    R.pairs[0].key = K;
    R.pairs[0].offset = L_offset;
    R.pairs[1].offset = P; 

    head head_data;
    fseek(tree_file, 0, SEEK_SET);
    fread(&head_data, sizeof(head), 1, tree_file);

    head_data.node_count +=1;
    head_data.level_count +=1;
    head_data.root_offset = head_data.node_count;
    
    fseek(tree_file, sizeof(head)+BLOCK_LENGTH*head_data.node_count, SEEK_SET);
    fwrite(&R, sizeof(node), 1, tree_file);

    fseek(tree_file, 0, SEEK_SET);
    fwrite(&head_data, sizeof(head), 1, tree_file);
    return;
  }

  int parent = parent_position(L_offset);

  node parent_data;

  fseek(tree_file, sizeof(head)+parent*BLOCK_LENGTH, SEEK_SET);
  fread(&parent_data, sizeof(node), 1, tree_file);

  int pointer_quantity = count_pointers(parent_data);

  if (pointer_quantity < N) 
  {
    int i = 0;
    while (parent_data.pairs[i].offset != L_offset) i++;

    if (i == N-2)
    {
      parent_data.pairs[i].key = K;
      parent_data.pointer = P;
    }
    else
    {
      parent_data.pairs[i].key = K;
      parent_data.pairs[i+1].offset = P;
    }
    fseek(tree_file, -sizeof(node), SEEK_CUR);
    fwrite(&parent_data, sizeof(node), 1, tree_file);
  }
  else 
  {
    nodeTP TP;
    copy_parent(&parent_data, &TP, K, P);
    erase_pairs(&parent_data);

    node new_parent_data;
    erase_pairs(&new_parent_data); //precisava de erase

    copy_per_pointers(&parent_data, &TP, 0, (ceil(N/2.0)-1));
    int K_parent_index = ceil(N/2.0)-1; //modifiquei ceil(N/2)-1 para ceil(N/2);
    int K_parent = TP.pairs[K_parent_index].key;

    fseek(tree_file, sizeof(head) +BLOCK_LENGTH*parent, SEEK_SET);
    fwrite(&parent_data, sizeof(node), 1, tree_file);

    copy_per_pointers2(&new_parent_data, &TP, (ceil(N/2.0)), N); //modifiquei ceil(N/2) para ceil(N/2)+1

    head head_data;
    fseek(tree_file, 0, SEEK_SET);
    fread(&head_data, sizeof(head), 1, tree_file);

    int new_parent_data_postion = head_data.node_count +1;
    fseek(tree_file, new_parent_data_postion*BLOCK_LENGTH, SEEK_CUR);
    fwrite(&new_parent_data, sizeof(node), 1, tree_file);

    head_data.node_count += 1;
    fseek(tree_file, 0, SEEK_SET);
    fwrite(&head_data, sizeof(head), 1, tree_file);

    insert_in_parent(&parent_data, K_parent, new_parent_data_postion, parent);

  }
}  


void insert(int K, int P)
{
    parents.clear();
    head head_data;
    node node_data;
    int current_node;
    fseek(tree_file, 0, SEEK_SET);
    fread(&head_data, sizeof(head), 1, tree_file);     //Lê o cabeçalho no inicio do arquivo;
    if(head_data.root_offset == -1)                    //Verifica se já existe uma raiz
    {
        node empty_node;
        for(int i = 0; i < N-1; i++)                   // Era N-2
        {
            empty_node.pairs[i].key = -1;                   //inicia as chaves do novo bloco com -1
            empty_node.pairs[i].offset = -1;                   //inicia as chaves do novo bloco com -1
        }
        empty_node.pointer = -1;
        head_data.root_offset = 0;                          //Seta a raiz como o bloco 0
        head_data.level_count = 1;                          //Adiciona um level na árvore
        fwrite(&empty_node, sizeof(node), 1, tree_file);    //Escreve raiz vazia
        fseek(tree_file, 0, SEEK_SET);                      //Seta o cursor para o inicio do arquivo.
        fwrite(&head_data, sizeof(head), 1, tree_file);     //Atualiza o cabeçalho do arquivo
    }
    else                                                
    {
        fseek(tree_file, 0, SEEK_SET);
        fread(&head_data, sizeof(head), 1, tree_file);              //Lê o cabeçalho da raiz
        fseek(tree_file, (head_data.root_offset*BLOCK_LENGTH), SEEK_CUR);   //Acha a raiz
        
        int current_level = 1;   
        current_node = head_data.root_offset;                       //o level atual começa na raiz
        while(current_level != head_data.level_count)                   //Enquanto não chegou ao nó folha
        {
            int i = 0;                                                  //par P, K atual no nó
            fread(&node_data, sizeof(node), 1, tree_file);              //Carrega na memória o nó atual para busca
            parents.push_back(current_node);                            //salva uma lista com os nós visitados.
            while(true)
            {
                if(i == N-1 && node_data.pairs[i-1].key < K) {
                    current_node = node_data.pointer;
                    fseek(tree_file, sizeof(head)+current_node*BLOCK_LENGTH, SEEK_SET);  //Seta o próximo nó a ser lido no arquivo
                    current_level++;
                    break;
                }
                else if(node_data.pairs[i].key > K || node_data.pairs[i].key == -1)    //Se achou uma chave Ki que é maior que K ou se a maior chave no nó atual for menor que K, então aponta para o próximo nó 
                {
                    current_node = node_data.pairs[i].offset;
                    fseek(tree_file, sizeof(head)+ current_node*BLOCK_LENGTH, SEEK_SET);  //Seta o próximo nó a ser lido no arquivo
                    current_level++;
                    break;
                }
                i++;
            }
        }
    }
    parents.push_back(current_node);                            //salva uma lista com os nós visitados.
    fread(&node_data, sizeof(node), 1, tree_file);       //Lê o nó folha onde será inserido K e P
    int keys_quantity = count_keys(node_data);
    if(keys_quantity < N-1)
    {
        insert_in_leaf(&node_data, K, P, keys_quantity);
        fseek(tree_file, -sizeof(node), SEEK_CUR);
        fwrite(&node_data, sizeof(node), 1, tree_file);
    }
    else{
        node newL;
        erase_pairs(&newL);
        nodeT T;
        copy_all_pairs(&node_data, &T);
        insert_in_leaf(&T, K, P);
        
        newL.pointer = node_data.pointer;
        int newL_offset = head_data.node_count+1;
        head_data.node_count++; 
        node_data.pointer = newL_offset;
        erase_pairs(&node_data);
        copy_pairs(&node_data, &T, 0, ceil(N/2)-1);
        copy_pairs(&newL, &T, ceil(N/2), N-1);
        int minorK = newL.pairs[0].key;
        fseek(tree_file, -sizeof(node), SEEK_CUR);
        fwrite(&node_data, sizeof(node), 1, tree_file);
        fseek(tree_file, sizeof(head)+newL_offset*BLOCK_LENGTH, SEEK_SET);
        fwrite(&newL, sizeof(node), 1, tree_file);
        fseek(tree_file, 0, SEEK_SET);
        fwrite(&head_data, sizeof(head), 1, tree_file);

        insert_in_parent(&node_data, minorK, newL_offset, current_node);
        
    }
}

void print_result(find_data data)
{
  std::cout << "Id: " << data.art_data.ID << std::endl;
  std::cout << "Title: " << data.art_data.Titulo << std::endl;
  std::cout << "Year : " << data.art_data.Ano << std::endl;
  std::cout << "Authors: " << data.art_data.Autores << std::endl;
  std::cout << "Citations: " << data.art_data.Citacoes << std::endl;
  std::cout << "Update: " << data.art_data.Atualizacao << std::endl;
  std::cout << "Snippet: " << data.art_data.Snippet << std::endl;
  std::cout << "Blocos lidos: " << data.node_level << std::endl;
  std::cout << "Quantidade de blocos: " << data.node_count+1 << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

}

find_data find(int K)
{
    FILE * hash_file = fopen("../thayna/db-tp2/hash.bin", "r");
    controle_parental.clear();
    head head_data;
    node node_data;
    block_t block;
    int current_node;
    fseek(tree_file, 0, SEEK_SET);
    fread(&head_data, sizeof(head), 1, tree_file);     //Lê o cabeçalho no inicio do arquivo;
    if(head_data.root_offset == -1)                    //Verifica se já existe uma raiz
    {
      std::cout << "Error: Empty tree!" << std::endl;
    }
    else                                                
    {
        fseek(tree_file, 0, SEEK_SET);
        fread(&head_data, sizeof(head), 1, tree_file);              //Lê o cabeçalho da raiz
        fseek(tree_file, (head_data.root_offset*BLOCK_LENGTH), SEEK_CUR);   //Acha a raiz
        
        int current_level = 1;   
        current_node = head_data.root_offset;                       //o level atual começa na raiz
        while(current_level != head_data.level_count)                   //Enquanto não chegou ao nó folha
        {
            controle_parental.push_back(current_node);
            int i = 0;                                                  //par P, K atual no nó
            fread(&node_data, sizeof(node), 1, tree_file);              //Carrega na memória o nó atual para busca
            parents.push_back(current_node);                            //salva uma lista com os nós visitados.
            while(true)
            {
                if(i == N-1 && node_data.pairs[i-1].key < K) {
                    current_node = node_data.pointer;
                    fseek(tree_file, sizeof(head)+current_node*BLOCK_LENGTH, SEEK_SET);  //Seta o próximo nó a ser lido no arquivo
                    current_level++;
                    break;
                }
                else if(node_data.pairs[i].key > K || node_data.pairs[i].key == -1)    //Se achou uma chave Ki que é maior que K ou se a maior chave no nó atual for menor que K, então aponta para o próximo nó 
                {
                    current_node = node_data.pairs[i].offset;
                    fseek(tree_file, sizeof(head)+ current_node*BLOCK_LENGTH, SEEK_SET);  //Seta o próximo nó a ser lido no arquivo
                    current_level++;
                    break;
                }
                i++;
            }
        }
        fread(&node_data, sizeof(node), 1, tree_file);
        int keys_quantity = count_keys(node_data);
        std::cout << "keys_quantity: " << keys_quantity << std::endl;
        for (int i = 0; i < keys_quantity; i++) {
          std::cout << i << " -> "<< node_data.pairs[i].key << std::endl;
        }
        for (int i = 0; i < keys_quantity; i++) {
          if (node_data.pairs[i].key == K)
          {
            int k_offset = node_data.pairs[i].offset;
            fseek(hash_file, k_offset*BLOCK_LENGTH, SEEK_SET); // Aponta para o bucket de acordo com a chave
            fread(&block, BLOCK_LENGTH, 1, hash_file); // Carrega o bloco para memória
            for (int j = 0; j < block.articles_quantity; j++) {
              std::cout << "block.id: " << block.articles[j].ID << std::endl;
              if (block.articles[j].ID == K)
              {
                find_data result = {block.articles[j], head_data.node_count, head_data.level_count};
                print_result(result);
                fclose(hash_file);
                return result;
              } 
            }
        
          }
        }
    // std::cout << " " << std::endl;
    // }
    // for(int i = 0; i < controle_parental.size(); i++)
    // {
    //   std::cout << "parent[" << i << "] = " << controle_parental[i] << std::endl; 
    }
    std::cout << "Não encontrou a chave" << std::endl;
    fclose(hash_file);
    return {{}, head_data.node_count, head_data.level_count};
}

int main()
{

    bool leitura = true;
    tree_file = fopen("./primarytree.bin", "w+"); //coloque w++

    head empty_head;
    empty_head.level_count = 0;
    empty_head.node_count = 0;
    empty_head.root_offset = -1;
    fwrite(&empty_head, sizeof(head), 1, tree_file);

    io::CSVReader<7,io::trim_chars<> , io::double_quote_escape<';','\"'>> sample("sample_final.csv");
    sample.set_header("ID","Titulo","Ano","Autores","Citacoes","Atualizacao","Snippet");
    
    tipo_artigo_leitura * ta_aux = (tipo_artigo_leitura*) malloc(sizeof(tipo_artigo_leitura));
    while (leitura) 
    { 
        try {
          if (leitura = sample.read_row(ta_aux->ID, ta_aux->Titulo,ta_aux->Ano, ta_aux->Autores, ta_aux->Citacoes, ta_aux->Atualizacao, ta_aux->Snippet)) {

              int id = std::stoi(ta_aux->ID);
              insert(id, hashify(id));
              std::cout << "Inserindo id: " << ta_aux->ID << std::endl;
          }
        } catch (io::error::too_few_columns) {}
    }
    free(ta_aux);

    // tree_file = fopen("./primarytree.bin", "r"); //coloque w++
    // find(2);

    fclose(tree_file);

    return 0;
}

