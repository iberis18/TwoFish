#include <ctime>
#include <iostream>

#include "TwoFish.h"
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

    z.b[0] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[0]), 0x14d) ^
        gfMod(gfMultiplication(y.b[1], MDS[1]), 0x14d) ^
        gfMod(gfMultiplication(y.b[2], MDS[2]), 0x14d) ^
        gfMod(gfMultiplication(y.b[3], MDS[3]), 0x14d));
    z.b[1] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[4]), 0x14d) ^
        gfMod(gfMultiplication(y.b[1], MDS[5]), 0x14d) ^
        gfMod(gfMultiplication(y.b[2], MDS[6]), 0x14d) ^
        gfMod(gfMultiplication(y.b[3], MDS[7]), 0x14d));
    z.b[2] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[8]), 0x14d) ^
        gfMod(gfMultiplication(y.b[1], MDS[9]), 0x14d) ^
        gfMod(gfMultiplication(y.b[2], MDS[10]), 0x14d) ^
        gfMod(gfMultiplication(y.b[3], MDS[11]), 0x14d));
    z.b[3] = static_cast<unsigned char>(gfMod(gfMultiplication(y.b[0], MDS[12]), 0x14d) ^
        gfMod(gfMultiplication(y.b[1], MDS[13]), 0x14d) ^
        gfMod(gfMultiplication(y.b[2], MDS[14]), 0x14d) ^
        gfMod(gfMultiplication(y.b[3], MDS[15]), 0x14d));

    return (z.dw);
}


string TwoFish::generateKey()
{
    string s;
    srand((time(nullptr)));

    static constexpr char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < 32; ++i)
        s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return s;
}

void TwoFish::createKeys(const vector<unsigned int>& key)
{
    unsigned int i, j, k;
    unsigned char* m, * S;

    //группирование байт ключа для генерации вектора S
    m = (unsigned char*)key.data();

    //Мe = (М0, М2, ..., М 2К - 2) (четные)
    //Мo = (М1, М3, ..., М 2К - 1) (нечетные)
    //недостающие до ближайшего N биты ключа назначаются равными 0
    for (int i = 0, j = 0; i < 4, j < 8; i++, j += 2)
    {
        if (j > key.size())
        {
            Me[i] = 0x00000000;
            Mo[i] = 0x00000000;
            continue;
        }
        Me[i] = key[j];
    	Mo[i] = key[j + 1];
    }

    //Третий вектор S так же формируется из исходного ключа.
    //Состоит из k 32-битных слов.
    //Исходные байты ключа разбиваем на к групп по 8 байтов.
    //Каждую группу умножаем как вектор на RS-матрицу (матрица 4*8 Рида-Соломона) в поле Галуа GF(2^8).

    for (i = 0; i < 4; i++)
    {
        S = (unsigned char*)&(this->S[3 - i]);
        for (j = 0; j < 4; j++)
        {
            S[j] = static_cast<unsigned char>(gfMod(gfMultiplication(RS[j << 3], m[i << 3]), 0x14d));
            for (k = 1; k < 8; k++)
                S[j] ^= gfMod(gfMultiplication(RS[(j << 3) + k], m[(i << 3) + k]), 0x14d);
        }
    }

    //генерация 40 расширенных ключей
    for (i = 0; i < 20; i++)
    {
	    unsigned int ai = H(p * (i << 1), Me);
        unsigned int bi = ROL32(H(p * ((i << 1) + 1), Mo), 8);

        Key[i << 1] = ai + bi;
        Key[(i << 1) + 1] = ROL32(ai + (bi << 1), 9);
    }
}

void TwoFish::encrypt(const vector<unsigned>& src, vector<unsigned>& dst)
{
    //байты 128-ого блока исходной информации
    unsigned int p[4];
    //Входное забеливание 
    p[0] = src[0] ^ Key[0];
    p[1] = src[1] ^ Key[1];
    p[2] = src[2] ^ Key[2];
    p[3] = src[3] ^ Key[3];

    //цикл на 16 раундов
    for (unsigned int round = 0; round < 16; round++)
    {
	    //Функция G

	    const unsigned int g0 = H(p[0], S);

	    const unsigned int g1 = H(ROL32(p[1], 8), S);

        //Блоки  трансформируются с помощью PHT
        unsigned int f0 = g0 + g1 + Key[round * 2 + 8];
        unsigned int f1 = g0 + (g1 << 1) + Key[round * 2 + 9];

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
    dst.push_back(p[2] ^ Key[4]);
    dst.push_back(p[3] ^ Key[5]);
    dst.push_back(p[0] ^ Key[6]);
    dst.push_back(p[1] ^ Key[7]);
}

void TwoFish::decrypt(const vector<unsigned>& src, vector<unsigned>& dst)
{
    unsigned int p[4];

    //Отмена выходного забеливания
    p[2] = src[0] ^ Key[4];
    p[3] = src[1] ^ Key[5];
    p[0] = src[2] ^ Key[6];
    p[1] = src[3] ^ Key[7];

    //Отмена 16 раундов
    for (int round = 15; round >= 0; round--)
    {
	    unsigned int g0 = p[0];
        unsigned int g1 = p[1];
        p[0] = p[2];
        p[1] = p[3];
        p[2] = g0;
        p[3] = g1;

        g0 = H(p[0], S);
        g1 = H(ROL32(p[1], 8), S);

	    const unsigned int f0 = g0 + g1 + Key[round * 2 + 8];
	    const unsigned int f1 = g0 + (g1 << 1) + Key[round * 2 + 9];

        p[2] = ROL32(p[2], 1) ^ f0;
        p[3] ^= f1;
        p[3] = ROR32(p[3], 1);
    }

    //Отмена входного забеливания
    dst.push_back(p[0] ^ Key[0]);
    dst.push_back(p[1] ^ Key[1]);
    dst.push_back(p[2] ^ Key[2]);
    dst.push_back(p[3] ^ Key[3]);
}

