#pragma once
#include <vector>
#include <condition_variable>

template<class T>
class Position
{
public:
    T x;
    T y;

    Position(){}
    Position(T x, T y){this->x = x; this->y = y;}
    //Position(const Position<T>& other){x = other.x; y = other.y;}
    //void operator=(const Position<T>& other){x = other.x; y = other.y;}
    
    template<class T2>
    Position(const Position<T2>& other);

    template<class T2>
    void operator=(const Position<T2>& other);

    Position<T> operator-(const Position<T>& other){return Position<T>(x - other.x, y - other.y);}
    Position<T> operator+(const Position<T>& other){return Position<T>(x + other.x, y + other.y);}
    Position<T> operator/(const T& other){return Position<T>(x / other, y / other);}
    Position<T> operator*(const T& other){return Position<T>(x * other, y * other);}
};

template<class T>
template<class T2>
Position<T>::Position(const Position<T2>& other) {x = other.x; y = other.y;}

template<>
template<class T2>
Position<int>::Position(const Position<T2>& other){x = (int)other.x; y = (int)other.y;}

template<>
template<class T2>
Position<float>::Position(const Position<T2>& other){x = (float)other.x; y = (float)other.y;}

template<class T>
template<class T2>
void Position<T>::operator=(const Position<T2>& other) {x = other.x; y = other.y;}

template<>
template<class T2>
void Position<int>::operator=(const Position<T2>& other) {x = (int)other.x; y = (int)other.y;}

template<>
template<class T2>
void Position<float>::operator=(const Position<T2>& other) {x = (float)other.x; y = (float)other.y;}

template<class T>
class Average
{
    std::vector<T> samples;
    int count;
    int currentCount;
    int index;

public:
    Average(Average const&)         = delete;
    void operator=(Average const&)  = delete;

    Average(int count)
    {
        samples.resize(count);
        this->count = count;
        currentCount = 0;
        index = 0;
    }

    T getAverage()
    {
        T val = samples.front();
        for(int i = 1; i < currentCount; ++i)
        {
            val = val + samples[i];
        }

        return val / currentCount;
    }

    void addSample(const T& sample)
    {
        samples[index] = sample;
        index = (index + 1) % count;
        currentCount = std::min(currentCount + 1, count);
    }
};

struct Car
{
    float throtle; // -1(backwards) to 1(forward) -> 0.5 = 50% throtle forward
    float steering; // -1(left) to 1(right)
};

extern std::condition_variable stop;