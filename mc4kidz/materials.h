#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cassert>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include "array2d.h"

enum class Interaction : uint8_t { SCATTER = 0, FISSION = 1, CAPTURE = 2 };
class InteractionCDF {
public:
    InteractionCDF(float total, float scatter, float fission, float capture)
    {
        _cdf[0] = scatter / total;
        _cdf[1] = _cdf[0] + fission / total;
        _cdf[2] = _cdf[1] + capture / total; // just for illustration
        _cdf[2] = 1.0f;
    }

    // Sample an interaction from the CDF given a random float on [0, 1)
    Interaction sample(float r) const
    {
        if (r < _cdf[0]) {
            return Interaction::SCATTER;
        }

        if (r < _cdf[1]) {
            return Interaction::FISSION;
        }

        return Interaction::CAPTURE;
    }

private:
    std::array<float, 3> _cdf;
};

class ScatterCDF {
public:
    ScatterCDF(std::vector<float> scatter) : _cdf(scatter.size())
    {
        float total = std::accumulate(scatter.begin(), scatter.end(), 0.0f);
        float prev  = 0.0;
        for (int ig = 0; ig < scatter.size(); ++ig) {
            prev += scatter[ig];
            _cdf[ig] = prev / total;
        }
        return;
    }

    int sample(float r) const
    {
        int out_group = static_cast<int>(
            std::distance(_cdf.begin(), std::lower_bound(_cdf.begin(), _cdf.end(), r)));
        assert(out_group < 7);
        return out_group;
    }

private:
    std::vector<float> _cdf;
};

struct Material {
    Material(const std::string &name, const std::vector<float> &abs,
             const std::vector<float> &nfis, const std::vector<float> &fis,
             const std::vector<float> &chi,
             const std::vector<std::vector<float>> &scat);
    int ng;
    std::vector<float> xstr;
    std::vector<InteractionCDF> interaction_cdf;
    std::vector<ScatterCDF> scatter_cdf;
    std::vector<float> xsab;
    std::vector<float> xsnf;
    std::vector<float> xsf;
    std::vector<float> xsch;
    Array2D<float> xssc;
    std::string name;
};

class MaterialLibrary {
public:
    MaterialLibrary()
    {
        return;
    }

    void add_material(Material mat)
    {
        _ids_by_name[mat.name] = _materials_by_id.size();
        _materials_by_id.push_back(mat);
    }

    const Material &get_by_name(const std::string &name) const
    {
        return _materials_by_id[_ids_by_name.at(name)];
    }

    const Material &get_by_id(int id) const
    {
        return _materials_by_id[id];
    }

private:
    std::vector<Material> _materials_by_id;
    std::unordered_map<std::string, size_t> _ids_by_name;
};

MaterialLibrary C5G7();
