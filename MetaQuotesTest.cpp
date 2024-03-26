#include <iostream>
#include <cstring>
//#include <regex> - я так понял именно это и запрещено по условиям здания
//https://learn.microsoft.com/ru-ru/cpp/standard-library/regex-functions?view=msvc-170 

#include "CLogReader.h"
#include "SimpleTimer.h"

/*
Например:

Маска *abc*  отбирает все строки содержащие abc и начинающиеся и заканчивающиеся любой последовательностью символов.
Маска abc*  отбирает все строки начинающиеся с abc и заканчивающиеся любой последовательностью символов.
Маска abc?  отбирает все строки начинающиеся с abc и заканчивающиеся любым дополнительным символом.
Маска abc   отбирает все строки которые равны этой маске.
*/


int main(int argc, char* argv[])
{
	SimpleTimer timer;						// добавил простой таймер для замера результатов оптимизации

	char* file;
    char* filter;


	if (argc < 3) 
	{
		printf("\tWrong parameters! Example: MetaQuotesTest 20190102.log order*");
	}
	else 
	{
		file = argv[1];
		filter = argv[2];

		CLogReader mylogreader(file);
				
		if (mylogreader.Open()) 
		{
			mylogreader.SetFilter(filter); // с фильтром определились

			char buf[4096];
			while (mylogreader.GetNextLine(buf, sizeof(buf))) 
			{
				printf("%s\n", buf);		// если нашли, передаём в консоль
			}
		}
		else 
		{
			printf("Error opening file: %s\n", file);
		}
	}

    return 0;
}

