#include <iostream>
#include <vector>

template <typename T>
static void vector_move(std::vector<T> & from, std::vector<T> & to)
{
    to = static_cast<std::vector<T>&&>(from);
}

template <typename T>
static void vector_print(std::vector<T> & vector, const char * tittle)
{
    std::cout << "&" << tittle << " = " << &vector;
    if(vector.size())
    {
        std::cout << "  &" << tittle << "[0] = " << (void *)&vector[0] << "  " << tittle << " = [";
        for(int index = 0; index < vector.size(); index++)
        {
            std::cout << vector[index] << ((index != (vector.size() - 1)) ? ", " : "");
        }
        std::cout << "]";
    }
    else
    {
        std::cout << "  " << tittle << " = []";
    }
    std::cout << std::endl;
}

int main(int argc, char * argv[])
{
    try
    {
        std::vector<int> vector_1 = {1, 2, 3, 4, 5};
        std::vector<int> vector_2 = {32, 64, 128, 256, 1024};
        vector_print(vector_1, "vector_1");
        vector_print(vector_2, "vector_2");
        std::cout << "vector1 --> vector_2\n";
        vector_move(vector_1, vector_2);
        vector_print(vector_1, "vector_1");
        vector_print(vector_2, "vector_2");
    }
    catch(...)
    {
        std::cout << "ERROR!\n";
    }

    return 0;
}
