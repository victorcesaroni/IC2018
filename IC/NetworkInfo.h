#pragma once

// classe que define um objeto que armazena informa��es de uma rota
class PathInfo
{
public:
	PathInfo();

	/// from: identificador do n� de origem
	/// to: identificador do n� de destino
	/// hops: lista que possui, em ordem, os identificadores dos n�s de cada salto do caminho, exceto o indentifcador do n� de origem
	PathInfo(int from, int to, const std::vector<int>& hops);

	int from;
	int to;
	std::vector<int> hops; 
};

// classe que define um objeto que armazena informa��es da aloca��o do comprimento de onda
class LambdaAllocInfo
{
public:

	/// from: identificador do n� de origem do caminho para o qual o lambda foi alocado
	/// to: identificador do n� de destino do caminho para o qual o lambda foi alocado
	/// lambda: numero do lambda alocadoo
	LambdaAllocInfo(int from, int to, int lambda);

	int from;
	int to;
	int lambda;
};

