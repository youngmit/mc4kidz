#pragma once
#include <vector>

#include "simple_structs.h"

template <typename T> class PieChart {
public:
    PieChart(const std::vector<T> &data) : _data(data)
    {
        return;
    }

	void draw() const;

private:
    std::vector<T> _data;
};