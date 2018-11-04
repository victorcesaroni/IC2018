#pragma once

// classe que define um objeto que armazena informaÇões de uma rota
class PathInfo
{
public:
	PathInfo();

	/// from: identificador do nó de origem
	/// to: identificador do nó de destino
	/// hops: lista que possui, em ordem, os identificadores dos nós de cada salto do caminho, exceto o indentifcador do nó de origem
	PathInfo(int from, int to, const std::vector<int>& hops);

	int from;
	int to;
	std::vector<int> hops; 
};

// classe que define um objeto que armazena informações da alocação do comprimento de onda
class LambdaAllocInfo
{
public:

	/// from: identificador do nó de origem do caminho para o qual o lambda foi alocado
	/// to: identificador do nó de destino do caminho para o qual o lambda foi alocado
	/// lambda: numero do lambda alocadoo
	LambdaAllocInfo(int from, int to, int lambda);

	int from;
	int to;
	int lambda;
};

