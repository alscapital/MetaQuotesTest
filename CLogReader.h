#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

/// <summary>
/// ����� ��� ������������ ������ �����
/// </summary>
class CLogReader final							// ����� �� �������������
{
public:
	
	CLogReader(const char* filename);
	~CLogReader();

	CLogReader() = delete;						// �� ��������� �� �����

	bool    Open();								// �������� �����, false - ������
	void    Close();							// �������� �����

	bool    SetFilter(const char* filter);		// ��������� ������� �����, false - ������
	bool    GetNextLine(char* buf,				// ������ ��������� ��������� ������,
						const int bufsize);  	// buf - �����, bufsize - ������������ �����
												// false - ����� ����� ��� ������

private:

	const char* filename_;						// ��������� ��� �����
	FILE* file_;
	bool fileOpened_;							// ���� �������� �����

	char* filter_;								// ��������� �������

	// returns next read line from file in a_result, size in a_size
	char* ReadNextLine();
	bool IsFilterEqualCurrentString(const char* str, const char* filter);
};

