#pragma once

#include <vector>

template<typename T>
class Array2D {
public:
	Array2D(size_t n_row, size_t n_col): _n_row(n_row), _n_col(n_col), _data(n_row*n_col) {
		return;
	}

	Array2D(size_t n_row, size_t n_col, T init) : _n_row(n_row), _n_col(n_col), _data(n_row*n_col, init) {
		return;
	}

	const T &operator()(size_t i, size_t j) const {
		return _data[_n_col*i + j];
	}

	T &operator()(size_t i, size_t j) {
		return _data[_n_col*i + j];
	}

	size_t extent(int dim) const {
		if (dim == 0) {
			return _n_row;
		}
		else {
			return _n_col;
		}
	}
private:
	size_t _n_row;
	size_t _n_col;
	std::vector<T> _data;
};
