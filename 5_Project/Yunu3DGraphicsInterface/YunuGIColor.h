#pragma once

namespace yunuGI
{
    union Color
    {
        float rgba[4]{0};
        struct 
        {
            float r, g, b, a;
        };
    };
}