#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <iostream>
#include <vector>

/* class BufferManager
{
public:
    template <typename T>
    void appendVector(const std::vector<T> vector)
    {
        outputBuffer.push_back((intptr_t)copyVectorToMemory(vector));
    };

    template <typename T>
    void appendVectorSize(const std::vector<T> vector)
    {
        outputBuffer.push_back(vector.size());
    };

    int *getOutput()
    {
        const int size = outputBuffer.size();
        int *output = (int *)malloc(size * sizeof(int));
        for (size_t i = 0; i < size; i++)
        {
            output[i] = outputBuffer[i];
        }
        return output;
    };

private:
    std::vector<int> outputBuffer;
    template <typename T>
    int *copyVectorToMemory(const std::vector<T> vector)
    {
        unsigned int size = vector.size() * sizeof(vector[0]);
        int *buffer = (int *)malloc(size);
        memcpy(buffer, &vector.front(), size);
        return buffer;
    };
}; */

#endif // BUFFERMANAGER_H