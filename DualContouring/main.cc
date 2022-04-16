#include "main.h"

class BufferController
{
private:
    // we store the pointers to each data that we want in this arraybuffer 
    vector<int> outputBuffer;
    int *copyVectorToMemory(const vector<vec3> vector)
    {
        unsigned int size = vector.size() * sizeof(vector[0]);
        int *buffer = (int *)malloc(size);
        memcpy(buffer, &vector.front(), size);
        return buffer;
    }

public:
    void appendVector(const vector<vec3> vector)
    {
        outputBuffer.push_back(vector.size());
        outputBuffer.push_back((intptr_t)copyVectorToMemory(vector));
    }
    // exporting the output buffer
    int *getOutputBuffer()
    {
        const int size = outputBuffer.size();
        int *output = (int *)malloc(size * sizeof(int));
        for (size_t i = 0; i < size; i++)
        {
            output[i] = outputBuffer[i];
        }
        return output;
    }
};

int *dualContouringSphere(int width, int height, int depth)
{
    vector<vec3> positions = DualContouring::generateVertices(10, 10, 10);

    // the order of appending data to the buffer is important,
    // we will read the data by the same order in javascript
    BufferController bufferController;
    bufferController.appendVector(positions);

    return bufferController.getOutputBuffer();
}
int main()
{

    cout << "\n"
         << dualContouringSphere(10, 10, 10) << "\n"
         << endl;
    return 0;
}