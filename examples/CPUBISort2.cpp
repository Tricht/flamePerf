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
            break; // Stoppt, wenn kein Tausch erforderlich ist
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
    const int sizeBubble = 5000;
    // Generiere Zufallsdaten
    for (int i = 0; i < sizeBubble; ++i)
    {
        int value = rand() % sizeBubble;
        dataBubble.push_back(value);
    }

    const int sizeInsertion = 30000;
    // Generiere Zufallsdaten
    for (int i = 0; i < sizeInsertion; ++i)
    {
        int value = rand() % sizeInsertion;
        dataInsertion.push_back(value);
    }

    auto start = std::chrono::high_resolution_clock::now();
    bubbleSort(dataBubble);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Bubble Sort dauerte "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    insertionSort(dataInsertion);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Insertion Sort dauerte "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    return 0;
}
