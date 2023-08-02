#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>


using namespace std;

unsigned int NumGroup;
int a0 = 0x67452301;   // A
int b0 = 0xefcdab89;   // B
int c0 = 0x98badcfe;   // C
int d0 = 0x10325476;


//величина сдвига для каждой операции
short int S[] = { 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
                  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
                  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
                  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 
};

const  unsigned int K[] = {
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
    0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
    0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
    0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
    0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
    0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
    0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
    0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
    0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
};


unsigned int* filling(ifstream& file) {
    file.seekg(0, ios::end);
    file.clear();
    unsigned int SizeFile = file.tellg();
    file.seekg(0);
    NumGroup = (SizeFile + 8) / 64 + 1;
    unsigned int* fileR = NULL;
    try {
        fileR = new unsigned int[NumGroup * 16];
    }
    catch (...) {
        return NULL;
    }
    for (int i = 0; i < NumGroup * 16; ++i) {
        fileR[i] = 0;	
    }
    int cnt = 0;
    for (int i=0; i < SizeFile;i++) {
        fileR[cnt] |= file.get() << ((i % 4) * 8);
        if ((i + 1) % 4 == 0) cnt++;
    }
    fileR[cnt] |= 0x80 << ((SizeFile % 4) * 8);
    fileR[NumGroup * 16 - 2] = SizeFile * 8;
    return fileR;
}

unsigned int shift(unsigned int Mes, short int S) {
    unsigned int res = Mes << S;
    res |= Mes >> (32 - S);
    return res;
}

string ToStr(unsigned int Num) {
    string plate = "0123456789abcdef";
    string res = "";
    string temp = "";
    for (int i = 0; i < 8; ++i) {
        unsigned int index = Num >> (i * 4) & 0xf;
        temp += plate[index];
        if ((i + 1) % 2 == 0) {
            reverse(temp.begin(), temp.end());
            res += temp;
            temp = "";
        }
    }
    return res;
}

string MD5(ifstream& file) {
    a0 = 0x67452301;   // A
    b0 = 0xefcdab89;   // B
    c0 = 0x98badcfe;   // C
    d0 = 0x10325476;
    unsigned int* fileR = filling(file);
    file.close();
    if (fileR == NULL) {
        cout << "Нехватает оперативной пямяти!" << endl;
        return "";
    }
    for (int i = 0; i < NumGroup; i++) {
        unsigned int mes[16];
        for (int j = 0; j < 16; ++j) {
            mes[j] = fileR[i * 16 + j];
        }
        int A = a0;
        int B = b0;
        int C = c0;
        int D = d0;
        for (int i = 0; i < 64; ++i) {
            int F, g;
            if (i < 16) {
                F = (B & C) | ((~B) & D);
                g = i;
            }
            else if (i < 32) {
                F = (D & B) | ((~D) & C);
                g = (5 * i + 1) % 16;
                //g = (5*(i - 16) + 1) % 16
            }
            else if (i < 48) {
                F = B ^ C ^ D;
                g = (3 * i + 5) % 16;
            }
            else if (i < 64) {
                F = C ^ (B | (~D));
                g = (7 * i) % 16;
            }
            unsigned int t = B + shift(A + F + mes[g] + K[i], S[i]);
            unsigned int temp = D;
            A = D;
            D = C;
            C = B;
            B = t;
        }
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }
    return ToStr(a0) + ToStr(b0) + ToStr(c0) + ToStr(d0);

}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    do {
        cout << "Введите название файла" << endl;
        string FileName;
        getline(cin, FileName);
        ifstream file;
        file.open(FileName, ios::binary);
        if (!file.is_open()) {
            cout << "Неудалось открыть файл" << endl;
            continue;
        }
        string Hash;
        if ((Hash = MD5(file)) == "") {
            continue;
        }
        cout << Hash << endl;
        a0 = 0x67452301;   // A
        b0 = 0xefcdab89;   // B
        c0 = 0x98badcfe;   // C
        d0 = 0x10325476;
    } while (true);

}