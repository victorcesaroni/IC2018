#pragma once

template<class T>
class Permutator
{
public:
	std::vector<std::vector<T>> permutations;

	Permutator(const std::vector<T>& v)
	{
		std::vector<T> tmp = {};
		P(tmp, v, v.size());
	}

	void P(const std::vector<T>& p, const std::vector<T>& r, unsigned n)
	{
		if (n == 0)
		{
			permutations.push_back(p);
		}
		else
		{
			for (unsigned i = 0; i < r.size(); i++)
			{
				std::vector<T> p_(p);
				std::vector<T> r_(r);
				p_.push_back(r[i]);
				r_.erase(r_.begin() + i);
				P(p_, r_, n - 1);
			}
		}
	}
};

int Factorial(int n)
{
	return (n == 1 || n == 0) ? 1 : Factorial(n - 1) * n;
}
