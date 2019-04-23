#pragma once
#include <memory>
#include <vector>

#include "state.h"
#include "drawable.h"

class InfoPane : public Drawable {
public:
    InfoPane(int width) : _width(width)
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
    std::vector<std::unique_ptr<Drawable>> _info;
};