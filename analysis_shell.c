#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define OBJSTRING_FILENAME "hotspot.string"
struct data_info
{
	int		len;
	char	addr[7];
	char	name[24];

}DATA_INFO_ARRAY[] = {
	{42,	"4154f1",	"msg1"},
	{1,		"415539",	"date"},
	{8,		"4154de",	"shll"},
	{3,		"41554f",	"inlo"},
	{15,	"415529",	"xecc"},
	{1,		"415525",	"lsto"},
	{22,	"41557d",	"tst1"},
	{22,	"415598",	"chk1"},
	{19,	"41553a",	"msg2"},
	{1,		"415552",	"rlax"},
	{1,		"4120e0",	"opts"},
	{11453,	"412616",	"text"},
	{19,	"415553",	"tst2"},
	{19,	"415568",	"chk2"},
	{256,	"4153ba",	"pswd"},
};

unsigned int asc_to_num(char c)
{
	if (c >= '0' && c <= '9')
	{
		return (c - '0');
	}

	if (c >= 'A' && c <= 'F')
	{
		return (10 + c - 'A');
	}

	if (c >= 'a' && c <= 'f')
	{
		return (10 + c - 'a');
	}
	printf("error,c=%c\n", c);

	return 0;
}


int analysis_data(char * addr, int len, char * data)
{
	FILE * fp;
	char buff[1024];
	char addrpre[6];
	int offset;
	char * psrc;
	int i = 0;

	if (strlen(addr) != 6)
	{
		printf("bad address: %s\n", addr);
		return -1;
	}
	offset = asc_to_num(addr[5]);
	//printf ("address offset=%d\n", offset);

	strncpy(addrpre, addr, 5);
	addrpre[5] = '\0';

	fp = fopen(OBJSTRING_FILENAME, "r");
	while(fgets(buff, sizeof(buff), fp) != NULL)
	{
		if (strstr(buff, addrpre) != NULL)
			break;
	}

	if (strlen(buff) < 43)
	{
		printf("error text:%s\n", buff);
		return -1;
	}

	psrc = buff + 8 + offset*2 + offset*2/8;

	while (i < len*4)
	{
		data[i++] = '\\';
		data[i++] = 'x';
		data[i++] = *psrc++;	
		data[i++] = *psrc++;
		if (*psrc == ' ') 
			psrc++;
		if (*psrc == ' ')
		{
			if (fgets(buff, sizeof(buff), fp) == NULL)
			{
				printf("fgets error!\n");
				fclose(fp);
				return -1;
			}
			psrc = buff + 8;
		}
	}
	//data[i++] = '\0';
	//printf("Result: data=%s\n", data);

	fclose(fp);

	return 0;
}
int main(int argc, char* argv[])
{
	int i;
	int result;
	FILE * fp;
	char * result_data = NULL;

	fp = fopen("unshc.h", "w");
	if (fp == NULL)
	{
		printf("error: open unshc.h\n");
		return 0;
	}

	for (i=0; i < sizeof(DATA_INFO_ARRAY)/sizeof(DATA_INFO_ARRAY[0]); ++i)
	{
		result_data = (char *)malloc(DATA_INFO_ARRAY[i].len*4);
		result = analysis_data(DATA_INFO_ARRAY[i].addr, DATA_INFO_ARRAY[i].len, result_data);
		if (result != 0)
		{
			printf("error: analysis %s,addr=%s\n", DATA_INFO_ARRAY[i].name, DATA_INFO_ARRAY[i].addr);
			continue;
		}
			

		fprintf(fp, "int %s_z = %d;\n", DATA_INFO_ARRAY[i].name,  DATA_INFO_ARRAY[i].len);
		fprintf(fp, "char %s[] = \"", DATA_INFO_ARRAY[i].name);
		fwrite(result_data, DATA_INFO_ARRAY[i].len*4, 1, fp);
		fprintf(fp, "\";\n\n");

		free(result_data);
		result_data = NULL;
	}
	
	fclose(fp);
	return 0;
}

