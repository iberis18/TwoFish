#include "TwoFish.h"
#include <ctime>
#include <iostream>

#include "GF2pow8.h"

using namespace std;

static unsigned char q0(const unsigned char x)
{
    unsigned char a0, b0, a1, b1, a2, b2, a3, b3, a4, b4;

    a0 = x >> 4;
    b0 = static_cast<unsigned char>(x & 0xf);
    a1 = a0 ^ b0;
    b1 = static_cast<unsigned char>(a0 ^ ROR4(b0, 1) ^ (a0 << 3));
    a2 = q0_t0[a1 & 0xf];
    b2 = q0_t1[b1 & 0xf];
    a3 = a2 ^ b2;
    b3 = static_cast<unsigned char>(a2 ^ ROR4(b2, 1) ^ (a2 << 3));
    a4 = q0_t2[a3 & 0xf];
    b4 = q0_t3[b3 & 0xf];

    return static_cast<unsigned char>((b4 << 4) | (a4 & 0xf));
}

static unsigned char q1(const unsigned char x)
{
    unsigned char a0, b0, a1, b1, a2, b2, a3, b3, a4, b4;

    a0 = x >> 4;
    b0 = static_cast<unsigned char>(x & 0xf);
    a1 = a0 ^ b0;
    b1 = static_cast<unsigned char>(a0 ^ ROR4(b0, 1) ^ (a0 << 3));
    a2 = q1_t0[a1 & 0xf];
    b2 = q1_t1[b1 & 0xf];
    a3 = a2 ^ b2;
    b3 = static_cast<unsigned char>(a2 ^ ROR4(b2, 1) ^ (a2 << 3));
    a4 = q1_t2[a3 & 0xf];
    b4 = q1_t3[b3 & 0xf];

    return static_cast<unsigned char>((b4 << 4) | (a4 & 0xf));
}

unsigned int H(unsigned int X, const unsigned int* L)
{
    union
	{
        unsigned int dw;
        unsigned int b[4];
    } z{}, y{};

    y.b[0] = static_cast<unsigned char>(X % 256);
    y.b[1] = static_cast<unsigned char>((X >> 8) % 256);
    y.b[2] = static_cast<unsigned char>((X >> 16) % 256);
    y.b[3] = static_cast<unsigned char>((X >> 24) % 256);

    y.b[0] = q1(y.b[0]);
    y.b[1] = q0(y.b[1]);
    y.b[2] = q0(y.b[2]);
    y.b[3] = q1(y.b[3]);
    y.dw ^= L[3];

    y.b[0] = q1(y.b[0]);
    y.b[1] = q1(y.b[1]);
    y.b[2] = q0(y.b[2]);
    y.b[3] = q0(y.b[3]);
    y.dw ^= L[2];

    y.b[0] = q0(y.b[0]);
    y.b[1] = q1(y.b[1]);
    y.b[2] = q0(y.b[2]);
    y.b[3] = q1(y.b[3]);
    y.dw ^= L[1];

    y.b[0] = q0(y.b[0]);
    y.b[1] = q0(y.b[1]);
    y.b[2] = q1(y.b[2]);
    y.b[3] = q1(y.b[3]);
    y.dw ^= L[0];

    y.b[0] = q1(y.b[0]);
    y.b[1] = q0(y.b[1]);
    y.b[2] = q1(y.b[2]);
    y.b[3] = q0(y.b[3]);

    z.b[0] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[0]), 0x169) ^
        gfMod(gfMultiplication(y.b[1], MDS[1]), 0x169) ^
        gfMod(gfMultiplication(y.b[2], MDS[2]), 0x169) ^
        gfMod(gfMultiplication(y.b[3], MDS[3]), 0x169)); 
    z.b[1] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[4]), 0x169) ^
        gfMod(gfMultiplication(y.b[1], MDS[5]), 0x169) ^
        gfMod(gfMultiplication(y.b[2], MDS[6]), 0x169) ^
        gfMod(gfMultiplication(y.b[3], MDS[7]), 0x169));
    z.b[2] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[8]), 0x169) ^
        gfMod(gfMultiplication(y.b[1], MDS[9]), 0x169) ^
        gfMod(gfMultiplication(y.b[2], MDS[10]), 0x169) ^
        gfMod(gfMultiplication(y.b[3], MDS[11]), 0x169));
    z.b[3] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[12]), 0x169) ^
        gfMod(gfMultiplication(y.b[1], MDS[13]), 0x169) ^
        gfMod(gfMultiplication(y.b[2], MDS[14]), 0x169) ^
        gfMod(gfMultiplication(y.b[3], MDS[15]), 0x169));

    return (z.dw);
}


string TwoFish::generateKey()
{
    string s;
    srand(static_cast<unsigned int>(time(nullptr))); //todo адекватный ранд

    static constexpr char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < 32; ++i)
        s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return s;
}

int TwoFish::createKeys(const vector<unsigned int>& key)
{
	//todo обработка длины ключа
    //ключ длины N = 128, 192, 256 бит.
    // if (key.size() > 8)
    // {
    //     std::cerr << "Key size does not match." << std::endl;
    //     return (-1);
    // }

    //группирование байт ключа для генерации вектора S
    unsigned char* m = (unsigned char*)key.data();

    //K = 4, 6, 8.
    //Мe = (М0, М2, ..., М 2К - 2) (четные)
    //Мo = (М1, М3, ..., М 2К - 1) (нечетные)
    //недостающие до ближайшего N биты ключа назначаются равными 0
    for (int i = 0, j = 0; i < 4, j < 8; i++, j += 2)
    {
        if (j > key.size())
        {
            this->Me[i] = 0x00000000;
            this->Mo[i] = 0x00000000;
            continue;
        }
        this->Me[i] = key[j];
        this->Mo[i] = key[j + 1];
    }

    //Третий вектор S так же формируется из исходного ключа.
    //Состоит из k 32-битных слов.
    //Исходные байты ключа разбиваем на к групп по 8 байтов.
    //Каждую группу умножаем как вектор на RS-матрицу (матрица 4*8 Рида-Соломона) в поле Галуа GF(2^8)

    for (unsigned int i = 0; i < 4; i++)
    {
        unsigned char* S = reinterpret_cast<unsigned char*>(&(this->S[3 - i]));
        for (unsigned int j = 0; j < 4; j++)
        {
            S[j] = static_cast<unsigned char>(gfMod(gfMultiplication(RS[j << 3], m[i << 3]), 0x14d));
            for (unsigned int k = 1; k < 8; k++)
                S[j] ^= gfMod(gfMultiplication(RS[(j << 3) + k], m[(i << 3) + k]), 0x14d);
        }
    }

    //генерация 40 расширенных ключей
    for (unsigned int i = 0; i < 20; i++)
    {
	    const unsigned int ai = H(p * (i << 1), this->Me);
	    const unsigned int bi = ROL32(H(p * ((i << 1) + 1), this->Mo), 8);

        this->Key[i << 1] = ai + bi;
        this->Key[(i << 1) + 1] = ROL32(ai + (bi << 1), 9);
    }

    this->init_ = 1;

    return(0);
}

int TwoFish::encrypt(const vector<unsigned>& src, vector<unsigned>& dst)
{
    //if (!this->init_)
    //{
    //    std::cerr << "Keys and messages not generated." << std::endl;
    //    return (-1);
    //}

    //байты 128-ого блока исходной информации
    unsigned int p[4];
    //Входное забеливание 
    p[0] = src[0] ^ this->Key[0];
    p[1] = src[1] ^ this->Key[1];
    p[2] = src[2] ^ this->Key[2];
    p[3] = src[3] ^ this->Key[3];

    //цикл на 16 раундов
    for (unsigned int round = 0; round < 16; round++)
    {
	    //Функция G

	    const unsigned int g0 = H(p[0], this->S);

	    const unsigned int g1 = H(ROL32(p[1], 8), this->S);

        //Блоки  трансформируются с помощью PHT
        unsigned int f0 = g0 + g1 + this->Key[round * 2 + 8];
        unsigned int f1 = g0 + (g1 << 1) + this->Key[round * 2 + 9];

        p[2] ^= f0;
        p[2] = ROR32(p[2], 1);
        p[3] = ROL32(p[3], 1) ^ f1;

        f0 = p[0];
        f1 = p[1];

        p[0] = p[2];
        p[1] = p[3];
        p[2] = f0;
        p[3] = f1;
    }

    //Выходное забеливание
    dst.push_back(p[2] ^ this->Key[4]);
    dst.push_back(p[3] ^ this->Key[5]);
    dst.push_back(p[0] ^ this->Key[6]);
    dst.push_back(p[1] ^ this->Key[7]);

	return 0;
}

int TwoFish::decrypt(const vector<unsigned>& src, vector<unsigned>& ds)
{

}

