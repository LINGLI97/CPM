#include <stxxl/vector>
#include <iostream>
int main()
{
    typedef stxxl::VECTOR_GENERATOR<int>::result vector;
    vector my_vector;
    for (int i = 0; i < 1024 * 1024; i++)
    {
        my_vector.push_back(i + 2);
    }
    std::cout << my_vector[99] << std::endl;
    my_vector[100] = 0;
    while (!my_vector.empty())
    {
        my_vector.pop_back();
    }
    return 0;
}