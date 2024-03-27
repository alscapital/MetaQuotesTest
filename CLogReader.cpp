#include "CLogReader.h"
#define LINE_BUFFER_SIZE 4096	// не превышаем верхнюю границу диапазона 1-4

/// <summary>
/// Конструктор
/// </summary>
/// <param name="filename"></param>
CLogReader::CLogReader(const char* filename):filename_(filename), file_(nullptr),fileOpened_(false),filter_(nullptr)
{
	
}
/// <summary>
/// Деструктор
/// </summary>
CLogReader::~CLogReader()
{
	Close();				// закрыли файл для чтения

	free(filter_);			// освобождаем блок памяти выделенный malloc
}
/// <summary>
/// Метод для открытия переданного файла
/// </summary>
/// <returns></returns>
bool CLogReader::Open()
{
	bool res;											// результат выполнения

	errno_t error = fopen_s(&file_, filename_, "rb");	// пробуем открыть с проверкой

	if (error == 0)										// если открылся, то
	{
		res = true;										// результат ок
	}
	else												// если не открылся
	{
		file_ = nullptr;								// очищаем пойнтер
		res = false;									// результат фолс
	}

	return res;											// возвращаем, что получилось
}
/// <summary>
/// Метод для закрытия файла
/// </summary>
void CLogReader::Close()
{
	if (file_ != nullptr)			// проверяем пойнтер на файл
	{
		fclose(file_);				// закрываем
	}
}
/// <summary>
/// Метод для определения фильтра поиска
/// </summary>
/// <param name="filter"></param>
/// <returns></returns>
bool CLogReader::SetFilter(const char* filter)
{
	bool res;										// переменная результата

	if (filter != nullptr)							// если пойнтер не нулевой
	{
		size_t length = strlen(filter) + 1;			// длинна строки

		filter_ = static_cast<char*>(malloc(length + 1)); // преобразовываем и при сбое не вызывает исключение

		if (filter_ != nullptr)						// проверяем указатель фильтра
		{
			strcpy_s(filter_, length, filter);		// копируем, вроде по требованиям можно так
			filter_[length - 1] = '\0';				// инициализация терминирующим нулём через индексирование

			res = true;								// ок
		}
		else										// если нет
		{
			res = false;							// всё
		}
	}
	else											// если нулевой
	{
		res = false;								// nullptr passed
	}

	return res;										// возвращаем что получилось
}
/// <summary>
/// Читаем следующую строку
/// </summary>
/// <returns></returns>
char* CLogReader::ReadNextLine()
{
	char* result = nullptr;					// указатель результата
	size_t maxlength = 0;					// максимальная длинна

	while (true)							// начинаем обработку
	{
		char* crlf, * block;				// указатели

		maxlength += LINE_BUFFER_SIZE;		// масимальная длинна кратна значению буфера

		result = static_cast<char*>(realloc(result, maxlength + 1)); // когда уже памяти недостаточно для выделения будет нулевой пойнтер

		if (result == nullptr)				// если в результате нулевой пойнтер
		{
			break;							// можно дальше не идти
		}

		block = result + maxlength - LINE_BUFFER_SIZE;				// вычисляем размер блока

		if (fgets(block, LINE_BUFFER_SIZE + 1, file_) == nullptr)	// ищим в файле (плюс один для терминирующего нуля)
		{
			if (block == result)									// 
			{
				free(result);										// освобождаем память
				result = nullptr;									// нулевой пойнтер
			}
			break;													// всё
		}

		crlf = strchr(block, '\n');									// ищем первое вхождение указателя на перевод на новую строку в блоке

		if (nullptr != crlf)										// пока строка не закончилась
		{
			*crlf = '\0';											// ставим терминирующий ноль
			if (crlf != block)										// пока не дошли до конца
			{
				if ('\r' == *(--crlf))								// в начало строки
					*crlf = '\0';									// ставим завершение
			}
			break;													// всё
		}
	}
	return result;													// возвращаем что получилось
}
/// <summary>
/// Сравнение строк и фильтров
/// </summary>
/// <param name="str"></param>
/// <param name="filter"></param>
/// <returns></returns>
bool CLogReader::IsFilterEqualCurrentString(const char* str, const char* filter)
{
	const char* cp = nullptr;										// 
	const char* mp = nullptr;										// то, что после звёздочки в фильтре

	while ((*str) && (*filter != '*'))								// идём до звездочки
	{
		if ((*filter != *str) && (*filter != '?'))					// если не совпало сразу и не символ вопроса
		{
			return false;											// значит не то
		}
		// можно перебирать дальше пока не закончится стринга
		++str;														// инкрементируем указатель
		++filter;													// инкрементируем указатель
	}
	// если на конце знак вопроса
	while (*str)													// или что то ещё есть
	{
		if (*filter == '*')											// если фильтр только звёздочка
		{
			if (!*++filter)											// и больше ничего нет
			{
				return true;										// нашли 
			}
			// если же ещё что то в фильтре есть
			mp = filter;											// смотрим что осталось
			cp = str + 1;											// смотрим что в строке после звездочки
		}
		else if ((*filter == *str) || (*filter == '?'))				// если не только звёздочка или же там только вопрос
		{
			//значит наконец-то мы дошли по оставшейся части цикла, ура
			++filter;												// смещаем фильтр
			++str;													// смещаем строку
		}
		else														// ещё не дошли до следующего условия фильтра справа
		{
			filter = mp;											// оставшийся фильтр
			str = cp++;												// что осталось сокращаем пока не получим фильтр
		}
	}

	while (*filter == '*')											// если оставшееся звездочка
	{
		++filter;													// идём сколько находим звездочек
	}

	return static_cast<bool>(!*filter);								// возвращаем результат
}
/// <summary>
/// Получаем следующую строку
/// </summary>
/// <param name="buf"></param>
/// <param name="bufsize"></param>
/// <returns></returns>
bool CLogReader::GetNextLine(char* buf, const int bufsize)
{
	bool res = false;									// результат
	char* line = nullptr;								// указатель на строку

	while ((line = ReadNextLine()) != nullptr)			// вытаскиваем по строкам
	{
		if (IsFilterEqualCurrentString(line, filter_))	// если нашли, 
		{
			break;										// дальше не идём, а отдаём line
		}

		free(line);										// освобождаем
		line = nullptr;									// нулевой пойнтер
	}

	if (line != nullptr)								// если нашли соответствие
	{
		size_t lineLength = strlen(line) + 1;			// +1 терминирующий ноль

		size_t sizeToWrite = (lineLength <= bufsize) ? lineLength : bufsize;	// сравниваем полученное значение с буфером

		errno_t error = strcpy_s(buf, sizeToWrite, line);	// копируем строку в буфер

		if (error == 0)										// если вернулось 0, то
		{
			res = true;										// значит нашли
		}

		free(line);											// освободили память
	}

	return res;												// вернули результат
}
