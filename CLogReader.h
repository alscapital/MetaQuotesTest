#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

/// <summary>
/// Класс для эффективного чтения логов
/// </summary>
class CLogReader final							// чтобы не наследовались
{
public:
	
	CLogReader(const char* filename);
	~CLogReader();

	CLogReader() = delete;						// по умолчанию не нужен

	bool    Open();								// открытие файла, false - ошибка
	void    Close();							// закрытие файла

	bool    SetFilter(const char* filter);		// установка фильтра строк, false - ошибка
	bool    GetNextLine(char* buf,				// запрос очередной найденной строки,
						const int bufsize);  	// buf - буфер, bufsize - максимальная длина
												// false - конец файла или ошибка

private:

	const char* filename_;						// указатель имя файла
	FILE* file_;
	bool fileOpened_;							// флаг открытия файла

	char* filter_;								// указатель фильтра

	// returns next read line from file in a_result, size in a_size
	char* ReadNextLine();
	bool IsFilterEqualCurrentString(const char* str, const char* filter);
};

