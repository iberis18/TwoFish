#include <fstream>
#include <iostream>
#include <vector>

#include "FileOperations.h"
#include "TwoFish.h"

using namespace std;


int main()
{
	setlocale(LC_ALL, "russian");
	TwoFish twofish;

	//чтение файла
	vector<unsigned int> arr = readFile("infile.bmp");

	//генерация ключа
	const string key = twofish.generateKey();
	cout << "Ключ: " << key << endl << endl;
	const vector<unsigned int> intKey = stringToInt(key);
	//создание сорока расширенных ключей
	twofish.createKeys(intKey);

	//Шифрование
	vector<unsigned int> encrypt;
	cout << "Шифрование..." << endl;
	for (int i = 0; i < arr.size(); i += 4)
		twofish.encrypt({ arr[i], arr[i + 1], arr[i + 2], arr[i + 3] }, encrypt);
	//запись в файл
	writeFile("encrypted.bmp", encrypt, true);
	cout << "Шифрование выполнено!" << endl << endl;

	//расшифровка
	arr = readFile("encrypted.bmp");
	vector<unsigned int> decrypt;
	cout << "Расшифровка..." << endl;
	for (int i = 0; i < encrypt.size(); i += 4)
		twofish.decrypt({ encrypt[i], encrypt[i + 1], encrypt[i + 2], encrypt[i + 3] }, decrypt);
	cout << "Расшифровка выполнена!" << endl << endl;

	//distribution(encrypt);

	writeFile("1_dec.bmp", decrypt);


	return 0;
}	