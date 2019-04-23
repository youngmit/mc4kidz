#pragma once
#include <vector>
#include <algorithm>

#include "drawable.h"
#include "state.h"

class Histogram : public Drawable {
public:
    void draw() const;

private:
    virtual std::vector<unsigned int> _get_data() const = 0;
};

class SpectrumHistogram : public Histogram {
public:
    SpectrumHistogram(const State *state) : _state(state)
    {
        return;
    }

private:
    const State *_state = nullptr;

    std::vector<unsigned int> _get_data() const override final
    {
        if (_state) {
            auto spectrum = _state->get_spectrum();
            std::reverse(spectrum.begin(), spectrum.end());
            return spectrum;
        }

        return {};
    }
};