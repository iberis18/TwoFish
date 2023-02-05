#pragma once
static unsigned short gfMultiplication(unsigned char a, unsigned char b)
{
	unsigned short res = 0;
    for (unsigned char x = 0x80; x; x >>= 1)
    {
        if (a & x)
        {
	        unsigned short tmp = static_cast<unsigned short>(0x0000 | b);
            if (x > 1)
            {
	            int shift = 1;
                while ((1 << shift) != x)
                    shift++;
                tmp <<= shift;
            }
            res ^= tmp;
        }
    }
    return (res);
}

static unsigned short gfMod(unsigned short a, unsigned short b)
{
	if (b == 0)
        return (0);

    unsigned short div = b;
    while (!(div & 0x8000))
        div <<= 1;

    unsigned short x = 0x8000;
    while (true)
    {
        if (a & x)
            a ^= div;
        if (div & 1)
            break;
        x >>= 1;
        div >>= 1;
    }
    return (a);
}