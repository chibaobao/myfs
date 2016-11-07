#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

void get(char *out)
{
	cJSON *root = NULL;
	cJSON *name = NULL;
	cJSON *girls_array =NULL;
	cJSON *tmp =NULL;
	root = cJSON_Parse(out);
	int i = 0;

	name = cJSON_GetObjectItem(root,"name");
	printf("%s = %s\n",name->string,name->valuestring);
	
	girls_array = cJSON_GetObjectItem(root,"girls");
	int size = cJSON_GetArraySize(girls_array);

	for(i = 0;i<size ; i++)
	{
		tmp = cJSON_GetArrayItem(girls_array,i);
		printf("girls_array[%d] :%s\n",i,tmp->valuestring);
	}

	cJSON_Delete(root);//注意释放root时girls_array也会被释放
}
int main(int argc, char *argv[])
{
	cJSON *root;//{}
	cJSON *girls_array;//[]

	root = cJSON_CreateObject();//{}
	cJSON_AddStringToObject(root, "name", "zhang3");
	cJSON_AddNumberToObject(root, "age", 18);
	cJSON_AddNumberToObject(root, "id", 200);

	girls_array = cJSON_CreateArray();//[]
	cJSON_AddItemToArray(girls_array, cJSON_CreateString("marong"));
	cJSON_AddItemToArray(girls_array, cJSON_CreateString("fengjie"));
	cJSON_AddItemToArray(girls_array, cJSON_CreateString("chunbro")); //["marong", "fengjie", "chunbro"]

	cJSON_AddItemToObject(root, "girls", girls_array);
	char *out = NULL;
	out =cJSON_Print(root);
	cJSON_Delete(root);//注意释放root时girls_array也会被释放
	printf("%s\n", out);
	printf("+++++++++++++++++++++++++++\n");
	get(out);

	free(out);
	return 0;
}

