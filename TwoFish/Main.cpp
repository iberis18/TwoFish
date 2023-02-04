#include <fstream>
#include <iostream>
#include <vector>

#include "FileOperations.h"
#include "TwoFish.h"

using namespace std;


int main()
{
	vector<unsigned int> arr = readFile("1.bmp");
	TwoFish twofish;

	string key = twofish.generateKey(); //генерация ключа
	cout << "Key:" << key << endl << endl;

	vector<unsigned int> keyDWORD = stringToInt(key);
	twofish.init(keyDWORD); //генерация ключей алгоритма

	return 0;
}	