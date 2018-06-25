//
//  main.cpp
//  simpleRSA
//
//  Created by Matthew Mccrackin on 6/18/18.
//  Copyright © 2018 Matthew Mccrackin. All rights reserved.
//

#include <iostream>
#include <random>

using namespace std;

// assumes positive input for the integers as prime numbers are +
long long int mod(long long int a, long long int b){
    return a - b*(a / b);
}

// assume that a > b
long long int gcd(long long int a, long long int b){
    long long int remainder = mod(a, b);
    if(remainder == 0){
        return b;
    }
    return gcd(b, remainder);
}

// iterate to square root of num and check if each integer is relatively prime with num
long long int phi(long long int num){
    long long int total = num;
    for(long long int i = 2; i * i <= num; i++){
        if(mod(num, i) == 0){
            while(mod(num, i) == 0){
                num = num / i;
            }
            total -= total / i;
        }
    }
    if(num > 1){
        total -= total / num;
    }
    return total;
}

long long int square_and_multiply(long long int num, long long int exp, long long int m){
    long long int res;
    int binary[64];
    long long int i;
    if(exp == 0){
        return 1;
    }
    for(i = 0; exp > 0; i++){
        if(mod(exp, 2) == 0){
            binary[i] = 0;
        }
        else{
            binary[i] = 1;
        }
        exp /= 2;
    }
    long long int j;
    for(j = i - 1; j >= 0; j--){
        if(binary[j] == 1){
            res = mod(num, m);
            break;
        }
    }
    for(j = j - 1; j >= 0; j--){
        if(binary[j] == 1){
            res = mod((mod(res * res, m) * num), m);
        }
        else{
            res = mod(res * res, m);
        }
    }
    return res;
}

int getR(long long int num, int &pow){
    num--;
    while(num > 0){
        num /= 2;
        pow++;
        if(num % 2 == 1){
            return num;
        }
    }
    return 0;
}

bool primeTest(long long int num){
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long int> dis(2, num-1);
    long long a;
    long long z;
    int pow = 0;
    int r = getR(num, pow);
    bool breakNext;
    
    for(int i  = 0; i < 10; i++){
        breakNext = false;
        a = dis(gen);   // choose a random from {2,3...,p-2}
        z = square_and_multiply(a, r, num);
        if(z != 1 && z != num - 1){
            for(int j = 1; j < pow; j++){
                z = mod(z*z, num);
                if(z == 1){
                    return false;   // composite
                }
                if(z == num - 1){
                    breakNext = true;
                    break;
                }
            
            }
            if(!breakNext){
                return false;
            }
        }
    }
    return true;
}

long long int generatePrime(){
    long long int num;
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long int> dis(4096, 65536);
    while(true){
        num = dis(gen);
        if(primeTest(num)){
            return num;
        }
    }
    return 0;
}

void generateKeys(long long int &n, long long int &e, long long int &d){
    long long int p = generatePrime();
    long long int q = generatePrime();
    n = p * q;
    long long int phiN = (p - 1) * (q - 1);
    for(long long int i = 2; i < phiN; i++){
        if(gcd(i, phiN) == 1){
            e = i;
            break;
        }
    }
    for(long long int i = max(p, q) + 1; i < phiN; i++){
        if(mod(i * e, phiN) == 1){
            d = i;
            break;
        }
    }
    cout << "Generated: kpub(n: " << n << " ,e: " << e
    << " ) d(" << d << ")" << endl;
}

long long int crypt(long long int n, long long int key, long long int text){
    return square_and_multiply(text, key, n);
}

long long int pow(long long int num, long long int pow){
    long long int res = 1;
    for(long long int i = 0; i < num; i++){
        res *= num;
    }
    return res;
}

long long int stringToNum(string str){
    long long int res = 0;
    bitset<8> cur;
    for(int i  = 0; i < 2; i++){
        cur = bitset<8>(str[i]);
        for(int j = 7; j >= 0; j--){
            if(cur[j] == 1){
                res += pow(2, i*8 + j);
            }
                
        }
    }
    return res;
}

string numToString(long long int num){
    bitset<64> set(num);
    bitset<8> cur;
    string str = "AA";
    for(int i  = 0; i < 2; i++){
        for(int j = 0; j < 8; j++){
            cur[j] = set[i*8 + j];
        }
        str[i] = (char)cur.to_ulong();
    }
    return str;
}

int main() {
    /*
    cout << "Begin large test case---\n";
    cout << "500,000 mod 500,001: " << mod(500000,500001) << endl;
    cout << "GCD(500,000 . 500,001)= " << gcd(500000,500001) << endl;
    cout << "Phi(193939)= " << phi(193939) << endl;
    cout << "Square and Multiply 1,000,000 squared mod 10,000,000: " <<
    square_and_multiply(1000000, 2, 10000000) << endl;
    cout << "Is 193939 prime? " << primeTest(193939) << endl;
    long long int p;
    p = generatePrime();
    cout << "Generated prime " << p << endl;
    cout << "Verifying primality: " << primeTest(p) << endl << endl;
    
    // modulus operation test
    cout << "Begin modulus operation test---\n";
    cout << "2 mod 1: " << mod(2, 1);
    cout << "\n5 mod 2: " << mod(5,2);
    cout << "\n7 mod 3: " << mod(7,3) << endl;

    // gcd test
    cout << "\n\n\nBegin gcd operation test---\n";
    cout << "GCD(10,8)= " << gcd(10,8);
    cout << "\nGCD(10,5)= " << gcd(10,5);
    cout << "\nGCD(2017, 13)= " << gcd(2017, 13) << endl;
    
    // phi test
    cout << "\n\n\nBegin phi test---\n";
    for(int i = 1; i < 30; i++){
        cout << i << " " << phi(i) << endl;
    }

    // square_and_multiply test
    cout << "\n\n\nBegin square_and_multiply test---\n";
    for(int i  = 1; i < 5; i++){
        for(int j = 0; j < 5; j++){
           cout << i << " to the " << j << "th power: " << square_and_multiply(i, j, 100) << endl;
        }
        cout << endl;
     }
     
    // getR test
    cout << "\n\n\nBegin getR test---\n";
    int pow;
    for(int i = 1; i < 53; i += 2){
        cout << i - 1 << " - " << getR(i, pow) << endl;
    }
    
    // primality test
    cout << "\n\n\nBegin primality test---\n";
    for(int i = 3; i < 103; i += 2){
        if(primeTest(i)){
            cout << i << endl;
        }
    }
    
    cout << "\n\nGenerating random primes---\n\n";
    for(int i  = 0; i < 10; i++){
        cout << generatePrime() << endl;
    }
     */
    long long int n, e, d;
    generateKeys(n, e, d);
    string str;
    long long int crypted;
    while(true){
        while(true){
            cout << "\nEnter a string to encrypt: ";
            cin >> str;
            cout << endl;
            if(str.length() == 2){
                break;
            }
            else if (str == "QQQ"){
                cout << "\nBye bye!\n";
                return 0;
            }
        }
        crypted = crypt(n, e, stringToNum(str));
        cout << "Encrypted as: " << crypted << endl;
        crypted = crypt(n, d, crypted);
        str = numToString(crypted);
        cout << "Decrypted as: " << str << endl;
        
    }
    return 0;
}
