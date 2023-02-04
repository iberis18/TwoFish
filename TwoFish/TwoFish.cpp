#include "TwoFish.h"
#include <ctime>

using namespace std;


string TwoFish::generateKey()
{
    string s;
    srand(static_cast<unsigned int>(time(nullptr))); //todo адекватный ранд

    static constexpr char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < 32; ++i)
        s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return s;
}

void TwoFish::init(const vector<unsigned>& vector)
{
}
