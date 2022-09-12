#pragma once

struct Position
{
    int x;
    int y;
};

struct Car
{
    float throtle; // -1(backwards) to 1(forward) -> 0.5 = 50% throtle forward
    float steering; // -1(left) to 1(right)
};