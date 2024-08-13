#include <iostream>
#include <vector>

typedef unsigned int UINT;
typedef unsigned char UCHAR;

static const unsigned long Ti[]{
	0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
	0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
	0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
	0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391,
};
static const unsigned long *Tin = Ti;
static const unsigned short Mj[]{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12,
	5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2,
	0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9,
};
static const unsigned short*Mjn = Mj;
static const unsigned int Se[]{
		7, 12, 17, 22,
		5, 9 , 14, 20,
		4, 11, 16, 23,
		6, 10, 15, 21,
};
static const unsigned int Yl[]{
		0, 1, 2, 3,
		3, 0, 1, 2,
		2, 3, 0, 1,
		1, 2, 3, 0,
};
static unsigned long A = 0x67452301L, B = 0xEFCDAB89L, C = 0x98BADCFEL, D = 0x10325476L;

static UINT F(UINT X, UINT Y, UINT Z) { return ((X & Y) | ((~X) & Z)); }
static UINT G(UINT X, UINT Y, UINT Z) { return ((X & Z) | (Y & (~Z))); }
static UINT H(UINT X, UINT Y, UINT Z) { return (X ^ Y ^ Z); }
static UINT I(UINT X, UINT Y, UINT Z) { return (Y ^ (X | (~Z))); }
static std::vector<UINT(*)(UINT, UINT, UINT)>Fu{ F,G,H,I };

#define S_TO_B(p) ((p << 24) + (((p << 16) >> 24) << 16) +(((p << 8) >> 24) << 8) + (p >> 24))
#define BIT_MV(val, n) (val >> (256 - n % 256) | (val << (n % 256)))

static void Alg(unsigned int* val) {
	unsigned long Ds[]{ A, B, C, D };
	unsigned long* a = Ds, * b = Ds + 1, * c = Ds + 2, * d = Ds + 3;

	for (int x = 0; x < 4; x++) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				unsigned char n[4]{ Yl[j * 4] ,Yl[j * 4 + 1],Yl[j * 4 + 2],Yl[j * 4 + 3] };
				unsigned long* a = Ds + n[0], * b = Ds + n[1], * c = Ds + n[2], * d = Ds + n[3];
				*a = *a + (Fu[x](*b, *c, *d) + val[*Mjn++] + *Tin++);
				*a = BIT_MV(*a, Se[j + x * 4]);
				*a = *a + *b;
			}
		}
	}

	Mjn = Mj;
	Tin = Ti;
	A += Ds[0]; B += Ds[1]; C += Ds[2]; D += Ds[3];
}

static std::string MD5_32Bit(std::string msg) {
	unsigned long long raw_len = msg.length() * 8;
	int fm = (448 - (raw_len % 512)) / 8;
	if (fm) { msg.push_back(0x80); fm--; }
	for (; fm > 0; fm--)msg.push_back(0);
	for (int i = 0; i < 8; i++)
		msg.push_back(((unsigned char*)&raw_len)[i]);

	for (int i = 0; i < msg.length() / 64; i++) {
		unsigned int val[16];
		memcpy(val, msg.data() + i * 64, 64);
		Alg(val);
	}

	char buf[4][10];
	unsigned long Ds[]{ S_TO_B(A), S_TO_B(B), S_TO_B(C), S_TO_B(D) };
	snprintf(buf[0], 10, "%8X", Ds[0]);  //A
	snprintf(buf[1], 10, "%8X", Ds[1]);  //B
	snprintf(buf[2], 10, "%8X", Ds[2]);  //C
	snprintf(buf[3], 10, "%8X", Ds[3]);  //D

	std::string _str;
	for (int y = 0; y < 4; y++)for (int z = 0; z < 8; z++) _str.push_back(buf[y][z]);
	return _str;
}