#include <iostream>
#include <vector>
#include <chrono>

void bubbleSort(std::vector<int> &arr)
{
    bool swapped;
    for (size_t i = 0; i < arr.size() - 1; i++)
    {
        swapped = false;
        for (size_t j = 0; j < arr.size() - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
            break;
    }
}

void insertionSort(std::vector<int> &arr)
{
    for (size_t i = 1; i < arr.size(); i++)
    {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int main()
{
    std::vector<int> dataBubble, dataInsertion;
    const int size = 10000;

    for (int i = 0; i < size; ++i)
    {
        int value = rand() % size;
        dataBubble.push_back(value);
        dataInsertion.push_back(value);
    }

    auto start = std::chrono::high_resolution_clock::now();
    bubbleSort(dataBubble);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Bubble Sort duration: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    insertionSort(dataInsertion);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Insertion Sort duration: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    return 0;
}
