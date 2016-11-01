#include <stdio.h>
#include <unistd.h>
int main()
{
	execlp("echo","echo","hello",NULL);
}
