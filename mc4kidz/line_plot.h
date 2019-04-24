#pragma once
#include <vector>

#include "drawable.h"
#include "state.h"

class LinePlot : public Drawable {
public:
    void draw() const;

private:
    virtual const std::vector<unsigned int> &get_data() const = 0;
};

class PopulationLinePlot : public LinePlot {
public:
    PopulationLinePlot(const State *state) : _state(state)
    {
        return;
    }

private:
    const State *_state;

    const std::vector<unsigned int> &get_data() const override final
    {
        return _state->get_population_history();
    }
};