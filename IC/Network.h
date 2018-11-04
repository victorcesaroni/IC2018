#pragma once

#include "NetworkInfo.h"

// classe que define um objeto para armazenar dados de uma conexão
class Link
{
public:
	/// to: nome do nó de destino dessa conexao
	/// cost: custo para chegar ate o destino
	Link(const std::string& to, int cost);

	std::string to;
	int cost;
};

// classe que define um nó de uma rede
class Node
{
public:
	Node();

	/// id: numero identificador do nó
	/// links: lista de conexoes que o nó possui
	Node(int id, const std::vector<Link>& links); //TODO:fixme

	int id; 
	std::vector<Link> links;
};

class Network
{
public:
	int nodeCount = 0; // número de nós na rede
	std::map<std::string, Node> nodes; // map que associa um nome de nó a um identificador de nó

	/// adiciona um nó na rede
	/// name: nome do nó
	/// links: lista de conexões que o nó possui
	void AddNode(const std::string& name, const std::vector<Link>& links);

	/// retorna o nome de um nó a partir de seu identificador
	/// id: identificador do nó
	std::string FindNameById(int id);

	/// exibe a matriz com os nomes dos nós colocando 'XXX' em valores infinitos
	/// matrix: matriz onde o número da linha e coluna é um identificador de nó da rede
	void PrintNodeMatrixNotInf(const Matrix<int>& matrix);

	/// exibe a matriz com os nomes dos nós
	/// matrix: matriz onde o número da linha e coluna é um identificador de nó da rede
	void PrintNodeMatrix(const Matrix<int>& matrix);

	/// exibe os valores da matriz agrupando-os em pares de linha e coluna
	/// matrix: matriz onde o número da linha e coluna é um identificador de nó da rede
	void PrintPairMatrix(const Matrix<int>& matrix);

	/// exibe as alocações de comprimento de lambda agrupando-os em pares de linha e coluna da matriz
	/// matrix: matriz onde o número da linha e coluna é um identificador de nó da rede, a linha é o nó de origem e a coluna é o nó de destino
	void PrintLambdaPairMatrix(const Matrix<std::vector<LambdaAllocInfo>>& matrix);
};
