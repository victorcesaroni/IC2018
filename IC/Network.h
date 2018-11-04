#pragma once

#include "NetworkInfo.h"

// classe que define um objeto para armazenar dados de uma conex�o
class Link
{
public:
	/// to: nome do n� de destino dessa conexao
	/// cost: custo para chegar ate o destino
	Link(const std::string& to, int cost);

	std::string to;
	int cost;
};

// classe que define um n� de uma rede
class Node
{
public:
	Node();

	/// id: numero identificador do n�
	/// links: lista de conexoes que o n� possui
	Node(int id, const std::vector<Link>& links); //TODO:fixme

	int id; 
	std::vector<Link> links;
};

class Network
{
public:
	int nodeCount = 0; // n�mero de n�s na rede
	std::map<std::string, Node> nodes; // map que associa um nome de n� a um identificador de n�

	/// adiciona um n� na rede
	/// name: nome do n�
	/// links: lista de conex�es que o n� possui
	void AddNode(const std::string& name, const std::vector<Link>& links);

	/// retorna o nome de um n� a partir de seu identificador
	/// id: identificador do n�
	std::string FindNameById(int id);

	/// exibe a matriz com os nomes dos n�s colocando 'XXX' em valores infinitos
	/// matrix: matriz onde o n�mero da linha e coluna � um identificador de n� da rede
	void PrintNodeMatrixNotInf(const Matrix<int>& matrix);

	/// exibe a matriz com os nomes dos n�s
	/// matrix: matriz onde o n�mero da linha e coluna � um identificador de n� da rede
	void PrintNodeMatrix(const Matrix<int>& matrix);

	/// exibe os valores da matriz agrupando-os em pares de linha e coluna
	/// matrix: matriz onde o n�mero da linha e coluna � um identificador de n� da rede
	void PrintPairMatrix(const Matrix<int>& matrix);

	/// exibe as aloca��es de comprimento de lambda agrupando-os em pares de linha e coluna da matriz
	/// matrix: matriz onde o n�mero da linha e coluna � um identificador de n� da rede, a linha � o n� de origem e a coluna � o n� de destino
	void PrintLambdaPairMatrix(const Matrix<std::vector<LambdaAllocInfo>>& matrix);
};
