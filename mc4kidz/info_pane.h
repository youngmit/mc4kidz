#pragma once
#include <memory>
#include <vector>

#include "state.h"
#include "drawable.h"

class InfoPane : public Drawable {
public:
    InfoPane(int width, int margin=0) : _width(width), _margin(margin)
    {
        return;
    }

    void draw() const;

    void add_info(std::unique_ptr<Drawable> info)
    {
        _info.push_back(std::move(info));
    }

private:
    int _width;
    int _margin;
    std::vector<std::unique_ptr<Drawable>> _info;
};