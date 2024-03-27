#include "CLogReader.h"
#define LINE_BUFFER_SIZE 4096	// �� ��������� ������� ������� ��������� 1-4

/// <summary>
/// �����������
/// </summary>
/// <param name="filename"></param>
CLogReader::CLogReader(const char* filename):filename_(filename), file_(nullptr),fileOpened_(false),filter_(nullptr)
{
	
}
/// <summary>
/// ����������
/// </summary>
CLogReader::~CLogReader()
{
	Close();				// ������� ���� ��� ������

	free(filter_);			// ����������� ���� ������ ���������� malloc
}
/// <summary>
/// ����� ��� �������� ����������� �����
/// </summary>
/// <returns></returns>
bool CLogReader::Open()
{
	bool res;											// ��������� ����������

	errno_t error = fopen_s(&file_, filename_, "rb");	// ������� ������� � ���������

	if (error == 0)										// ���� ��������, ��
	{
		res = true;										// ��������� ��
	}
	else												// ���� �� ��������
	{
		file_ = nullptr;								// ������� �������
		res = false;									// ��������� ����
	}

	return res;											// ����������, ��� ����������
}
/// <summary>
/// ����� ��� �������� �����
/// </summary>
void CLogReader::Close()
{
	if (file_ != nullptr)			// ��������� ������� �� ����
	{
		fclose(file_);				// ���������
	}
}
/// <summary>
/// ����� ��� ����������� ������� ������
/// </summary>
/// <param name="filter"></param>
/// <returns></returns>
bool CLogReader::SetFilter(const char* filter)
{
	bool res;										// ���������� ����������

	if (filter != nullptr)							// ���� ������� �� �������
	{
		size_t length = strlen(filter) + 1;			// ������ ������

		filter_ = static_cast<char*>(malloc(length + 1)); // ��������������� � ��� ���� �� �������� ����������

		if (filter_ != nullptr)						// ��������� ��������� �������
		{
			strcpy_s(filter_, length, filter);		// ��������, ����� �� ����������� ����� ���
			filter_[length - 1] = '\0';				// ������������� ������������� ���� ����� ��������������

			res = true;								// ��
		}
		else										// ���� ���
		{
			res = false;							// ��
		}
	}
	else											// ���� �������
	{
		res = false;								// nullptr passed
	}

	return res;										// ���������� ��� ����������
}
/// <summary>
/// ������ ��������� ������
/// </summary>
/// <returns></returns>
char* CLogReader::ReadNextLine()
{
	char* result = nullptr;					// ��������� ����������
	size_t maxlength = 0;					// ������������ ������

	while (true)							// �������� ���������
	{
		char* crlf, * block;				// ���������

		maxlength += LINE_BUFFER_SIZE;		// ����������� ������ ������ �������� ������

		result = static_cast<char*>(realloc(result, maxlength + 1)); // ����� ��� ������ ������������ ��� ��������� ����� ������� �������

		if (result == nullptr)				// ���� � ���������� ������� �������
		{
			break;							// ����� ������ �� ����
		}

		block = result + maxlength - LINE_BUFFER_SIZE;				// ��������� ������ �����

		if (fgets(block, LINE_BUFFER_SIZE + 1, file_) == nullptr)	// ���� � ����� (���� ���� ��� �������������� ����)
		{
			if (block == result)									// 
			{
				free(result);										// ����������� ������
				result = nullptr;									// ������� �������
			}
			break;													// ��
		}

		crlf = strchr(block, '\n');									// ���� ������ ��������� ��������� �� ������� �� ����� ������ � �����

		if (nullptr != crlf)										// ���� ������ �� �����������
		{
			*crlf = '\0';											// ������ ������������� ����
			if (crlf != block)										// ���� �� ����� �� �����
			{
				if ('\r' == *(--crlf))								// � ������ ������
					*crlf = '\0';									// ������ ����������
			}
			break;													// ��
		}
	}
	return result;													// ���������� ��� ����������
}
/// <summary>
/// ��������� ����� � ��������
/// </summary>
/// <param name="str"></param>
/// <param name="filter"></param>
/// <returns></returns>
bool CLogReader::IsFilterEqualCurrentString(const char* str, const char* filter)
{
	const char* cp = nullptr;										// 
	const char* mp = nullptr;										// ��, ��� ����� �������� � �������

	while ((*str) && (*filter != '*'))								// ��� �� ���������
	{
		if ((*filter != *str) && (*filter != '?'))					// ���� �� ������� ����� � �� ������ �������
		{
			return false;											// ������ �� ��
		}
		// ����� ���������� ������ ���� �� ���������� �������
		++str;														// �������������� ���������
		++filter;													// �������������� ���������
	}
	// ���� �� ����� ���� �������
	while (*str)													// ��� ��� �� ��� ����
	{
		if (*filter == '*')											// ���� ������ ������ ��������
		{
			if (!*++filter)											// � ������ ������ ���
			{
				return true;										// ����� 
			}
			// ���� �� ��� ��� �� � ������� ����
			mp = filter;											// ������� ��� ��������
			cp = str + 1;											// ������� ��� � ������ ����� ���������
		}
		else if ((*filter == *str) || (*filter == '?'))				// ���� �� ������ �������� ��� �� ��� ������ ������
		{
			//������ �������-�� �� ����� �� ���������� ����� �����, ���
			++filter;												// ������� ������
			++str;													// ������� ������
		}
		else														// ��� �� ����� �� ���������� ������� ������� ������
		{
			filter = mp;											// ���������� ������
			str = cp++;												// ��� �������� ��������� ���� �� ������� ������
		}
	}

	while (*filter == '*')											// ���� ���������� ���������
	{
		++filter;													// ��� ������� ������� ���������
	}

	return static_cast<bool>(!*filter);								// ���������� ���������
}
/// <summary>
/// �������� ��������� ������
/// </summary>
/// <param name="buf"></param>
/// <param name="bufsize"></param>
/// <returns></returns>
bool CLogReader::GetNextLine(char* buf, const int bufsize)
{
	bool res = false;									// ���������
	char* line = nullptr;								// ��������� �� ������

	while ((line = ReadNextLine()) != nullptr)			// ����������� �� �������
	{
		if (IsFilterEqualCurrentString(line, filter_))	// ���� �����, 
		{
			break;										// ������ �� ���, � ����� line
		}

		free(line);										// �����������
		line = nullptr;									// ������� �������
	}

	if (line != nullptr)								// ���� ����� ������������
	{
		size_t lineLength = strlen(line) + 1;			// +1 ������������� ����

		size_t sizeToWrite = (lineLength <= bufsize) ? lineLength : bufsize;	// ���������� ���������� �������� � �������

		errno_t error = strcpy_s(buf, sizeToWrite, line);	// �������� ������ � �����

		if (error == 0)										// ���� ��������� 0, ��
		{
			res = true;										// ������ �����
		}

		free(line);											// ���������� ������
	}

	return res;												// ������� ���������
}
