#define _WIN32_WINNT 0x0400

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <Windows.h>
#include <WinCrypt.h>
#pragma comment(lib, "advapi32.lib")
#include <WinError.h>
#include <cstdlib>
#include <string>

using namespace std;

HCRYPTKEY getSessionKey()
{
	HCRYPTPROV hProv = 0;
	HCRYPTKEY hSessionKey = 0;

	// Получение контекста криптопровайдера
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		cout << "CryptAcquireContext Error";
		cin.get();
		//return 1;
	}
	else {
		std::cout << "Cryptographic provider initialized" << std::endl;
	}
	// Генерация сессионного ключа
	if (!CryptGenKey(hProv, CALG_RC2, 64 << 16, &hSessionKey)) {
		cout << "CryptGenKey Error";
		cin.get();
		//return 1;
	}
	else {
		std::cout << "Session key generated" << std::endl;
		std::cout << hSessionKey << std::endl;
	}
	return hSessionKey;
}

int errorReturn()
{
	cout << GetLastError() << endl;
	cin.get();
	return 1;
}

int main() {
	setlocale(LC_ALL, "Russian");


	HCRYPTKEY hSessionKey = getSessionKey();
	std::cout << hSessionKey << std::endl;
	ifstream ifs;
	string ifs_s;
	cout << "Введите адрес исходного .txt файла (например:K:\\One\\File\\1.txt):" << endl;
	cin >> ifs_s;
	ifs.open(ifs_s, ios::in | ios::binary);
	ofstream ofs;
	string ofs_s;
	cout << "Введите адрес выходного файла:" << endl;
	cin >> ofs_s;
	ofs.open(ofs_s, ios::out | ios::binary);
	ofstream ofs1;
	string ofs1_s;
	cout << "Введите адрес выходного файла 2:" << endl;
	cin >> ofs1_s;
	ofs1.open(ofs1_s, ios::out | ios::binary);
	//Если удалось открыть файл для чтения и для записи.
	if (!ifs.is_open() || !ofs.is_open()) {
		cout << "File is not exist" << endl;
	}
	else {
		const int buf3_size = 8;
		char buf3[buf3_size+8] = { 0 };
		DWORD dwDataLen = 0;
		CryptEncrypt(hSessionKey, 0, true, 0, 0, &dwDataLen, buf3_size);
		while (!ifs.eof())
		{
			ifs.read(buf3, buf3_size);
			bool flag = ifs.eof();

			cout << "Plain Text is ";
			for (int i = 0; i < buf3_size; ++i)
			{
				cout << buf3[i];
			}
			cout << endl;

			// Шифрование данных
			cout << dwDataLen << endl;
			if (!CryptEncrypt(hSessionKey, 0, flag, 0, (BYTE*)buf3, &dwDataLen, buf3_size+8)) {
				ifs.close();
				ofs.close();
				ofs1.close();
				cout << "CryptEncrypt Error" << endl;
				return errorReturn();
			}
			else {
				cout << "Encrypted text is ";
				for (size_t i = 0; i < dwDataLen; ++i)
				{
					cout << buf3[i];
				}
				ofs.write(buf3, dwDataLen);
				cout << endl;
			}

			// Дешифрование данных
			if (!CryptDecrypt(hSessionKey, 0, flag, 0, (BYTE*)buf3, &dwDataLen)) {
				ifs.close();
				ofs.close();
				ofs1.close();
				cout << "CryptDecrypt Error";
				return errorReturn();
			}
			cout << "Decrypted text is ";
			for (int i = 0; i < buf3_size; ++i)
			{
				cout << buf3[i];
			}
			ofs1.write(buf3, dwDataLen);
			cout << endl;
		}
		ifs.close();
		ofs.close();
		ofs1.close();
	}
	cin.get();
	cin >> ifs_s;
}