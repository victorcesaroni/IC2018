#pragma once

template<class T>
class Matrix
{
	T *data;
public:
	int N, M;

	Matrix();
	Matrix(int N, int M);
	~Matrix();

	void Create(int N, int M);
	void Fill(T val);

	T* operator[](const int& i) const;
};

template<class T>
Matrix<T>::Matrix()
	: data(NULL)
{

}

template<class T>
Matrix<T>::Matrix(int N, int M)
{
	Create(N, M);
}

template<class T>
void Matrix<T>::Create(int N, int M)
{
	if (data != NULL)
	{
		delete[] data;
		data = NULL;
	}

	this->N = N;
	this->M = M;

	data = new T[N * M];

	if (!data)
		throw new std::exception("Can't alloc matrix");
}

template<class T>
void Matrix<T>::Fill(T val)
{
	for (int i = 0; i < N * M; i++)
		data[i] = val;
}

template<class T>
Matrix<T>::~Matrix()
{
	delete[] data;
}

template<class T>
T* Matrix<T>::operator[](const int& i) const
{
	return &data[i * M];
}
