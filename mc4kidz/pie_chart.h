#pragma once
#include <vector>

#include "drawable.h"
#include "simple_structs.h"
#include "state.h"

template <typename T> class PieChart : public Drawable {
public:
    PieChart()
    {
        return;
    }

    void draw() const;

    virtual std::vector<T> get_data() const = 0;
};

class InteractionPieChart : public PieChart<unsigned int> {
public:
    InteractionPieChart(const State *state) : _state(state)
    {
        return;
    }

    std::vector<unsigned int> get_data() const
    {
        if (_state) {
            return _state->get_interaction_counts();   
		}
        return {};
    }

private:
    const State *_state;
};