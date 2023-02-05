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

	string key = twofish.generateKey();
	cout << "Key:" << key << endl << endl;

	vector<unsigned int> intKey = stringToInt(key);
	twofish.createKeys(intKey);

	return 0;
}	