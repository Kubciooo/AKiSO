#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <linux/limits.h>
#define BUFF_SIZE 1024
//gcc -m32 -fno-stack-protector zad1.c -o zad1 -lm
int val(char c)
{
    if (c >= '0' && c <= '9')
        return (int)c - '0';
    else
        return (int)c - 'A' + 10;
}

int toDeci(char *str, int base)
{
    int len = strlen(str);
    int power = 1;
    int num = 0;
    int i;
    int pom = 0;
    if(str[0]=='-')pom=1;
    for (i = len - 1; i >= pom; i--)
    {
        if (val(str[i]) >= base)
        {
           printf("Invalid Number");
           return -1;
        }

        num += val(str[i]) * power;
        power = power * base;
    }
    return pom==1? num*-1 : num;
}

void toString(char* out, int number, int base)
{
	int i = 63;
	int j = 0;
	if (number < 0) {
		number = 0 - number;
		j = 1;
		out[0] = '-';
	}
	do {
		out[i] = "0123456789ABCDEF"[number % base];
		i--;
		number = number/base;
	} while (number > 0);
	while (++i < 64) {
		out[j++] = out[i];
	}
	out[j] = '\0';
}

void myprintf(char * pattern, ...)
{
  //va_start
  char *p = (char *) &pattern + sizeof(pattern);

  for(int i = 0; i<strlen(pattern); i++)
  {
    if(pattern[i]=='%' && i < strlen(pattern)-1)
    {
        char output[BUFF_SIZE];
        switch (pattern[i+1])
        {
          case 'd':
          {
            int *number;
            number = ((int *)p);
            //va_next
            p+=sizeof(int);
            toString(output, *number, 10);
            write(1,output, strlen(output));
            i+=2;
            break;
          }

          case 'b':
          {
            int *number;
            number = ((int *)p);
            p+=sizeof(int);
            toString(output, *number, 2);
            write(1,output, strlen(output));
            i+=2;
            break;
          }

          case 'x':
          {
            int *number;
            number = ((int *)p);
            p+=sizeof(int);
            toString(output, *number, 16);
            write(1,output, strlen(output));
            i+=2;
            break;
          }

          case 's':
          {
            char *s;
            s = *((char **)p);
            p+=sizeof(char*);
            while(s[0]!='\0')
            {
                write(1,s,sizeof(char));
                s++;
            }
            i+=2;
            break;
          }

          default:
          {
            write(1,pattern[i],1);
            break;
          }
        }
    }
    //wypisz wszystko inne, niż %
    int a = write(1, &pattern[i], 1);
  }

  p = NULL;
  write(1, "\n\0", 3);
}

int myscanf(const char*pattern, ...)
{
  char* p = (char*)&pattern + sizeof(pattern);
  char *input = calloc(BUFF_SIZE, sizeof(char));
  int size = read(0,input,BUFF_SIZE);
  if(input[size-1] == '\n')input[size-1] = '\0';
  input[size]='\n';
  if(!strcmp(pattern, "%d"))
  {
    int* in;
    in = (int*)(*(int*)p);
    p+=sizeof(int*);
    *in = toDeci(input,10);
  }
  else if(!strcmp(pattern, "%b"))
  {
    int* in;
    in = (int*)(*(int*)p);
    p+=sizeof(int*);
    *in = toDeci(input,2);
  }
  else if(!strcmp(pattern, "%x"))
  {
    int* in;
    in = (int*)(*(int*)p);
    p+=sizeof(int*);
    *in = toDeci(input,16);
  }
  else if(!strcmp(pattern, "%s"))
  {
    char** in;
    in = (char **)(*(char**) p);
    *in = input;
    p += sizeof(*in);
  }
  p = NULL;
  return size;
}


int main()
{
    char str[] = "-A11";
    int base = 16;
    int a;
    char *b;
    myprintf("testowanie decimala: ");
    myscanf("%d",&a);
    myprintf("%d\n",a);
    myprintf("%b\n",a);
    myprintf("%x\n",a);

    myprintf("testowanie stringa: ");
    myscanf("%s",&b);
    myprintf("%s\n",b);
    myprintf("testowanie hexa: ");
    myscanf("%x",&a);
    myprintf("%d\n",a);
    myprintf("%b\n",a);
    myprintf("%x\n",a);
    myprintf("testowanie binary: ");
    myscanf("%b",&a);
    myprintf("%d\n",a);
    myprintf("%b\n",a);
    myprintf("%x\n",a);
    return 0;
}
