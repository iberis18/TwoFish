#pragma once

#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<unsigned int> readFile(const std::string& filePath)
{
    ifstream f(filePath, ios_base::in | ios_base::binary);

    vector<unsigned int> arr;
    f.seekg(0, ios::end);
    const int length = static_cast<int>(f.tellg());
    f.seekg(0, ios::beg);

    const auto sizeOfBuffer = length / sizeof(unsigned);

    arr.resize(static_cast<unsigned long>(sizeOfBuffer + 1));
    f.read(reinterpret_cast<char*>(arr.data()), length);

    if (arr.size() % 4 != 0)
        arr.resize(arr.size() + arr.size() % 4);
    return arr;
}

vector<unsigned int> stringToInt(const string& str)
{
    vector<unsigned int> arr;
    unsigned int num = 0;
    for (int i = 0; i < str.length(); i++)
    {
        if (i != 0 && i % 4 == 0)
        {
            arr.push_back(num);
            num = 0;
        }
        num = num | (unsigned char(str[i]) << ((i % 4) * 8));
    }
    arr.push_back(num);

    for (int i = 0; i < (arr.size() % 4); ++i)
        arr.push_back(0);

    return arr;
}

void writeFile(const string& filePath, vector<unsigned int> arr, bool encrypted = false)
{
    ofstream fs(filePath, ios_base::out | ios_base::binary);

    vector<unsigned> arrCopy(arr.size());
    copy(arr.begin(), arr.end(), arrCopy.begin());
    while (arrCopy.back() == 0)
        arrCopy.pop_back();

    if (encrypted)
    {
        fs.write(reinterpret_cast<const char*>(arrCopy.data()), arrCopy.size() * 4);
        return;
    }

    unsigned int back = arrCopy[arrCopy.size() - 1];
    arrCopy.pop_back();
    fs.write(reinterpret_cast<const char*>(arrCopy.data()), arrCopy.size() * 4);

    for (int i = 0; i < 4; i++)
    {
        int shift = 8 * i;
        auto r = back >> shift;
        r &= 255;
        if (r != 0)
        {
            char backChar = static_cast<unsigned char>(r);
            fs.write(&backChar, 1);
        }
        else
            break;
    }
}