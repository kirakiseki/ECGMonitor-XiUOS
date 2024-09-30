/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
* @file:    test_musl.c
* @brief:   a application of musl test function
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/11/8
*/

#include <transform.h>

#ifdef ADD_NUTTX_FEATURES

#if defined(CONFIG_MUSL_LIBC) && defined(CONFIG_FS_AUTOMOUNTER)

static void file_read_and_write(void)
{
    FILE *p;
    char s[] = "good luck to you!";
    char buffer[20];

    if((p = fopen("/mnt/sdcard/test.txt","w+")) == NULL)
    {
      printf("Can not open file!\n");
    }
    fwrite(s, sizeof(s) + 1, 1, p);
    fseek(p, SEEK_SET, 0);
    fread(buffer, sizeof(buffer) + 1, 1, p);
    printf("read string is: %s\n", buffer);
    fclose(p);
}

static void file_operations(void)
{
   int ret;
   FILE *fp;
   char filename1[] = "/mnt/sdcard/file1.txt";
   char filename2[] = "/mnt/sdcard/file2.txt";

   fp = fopen(filename1, "w");

   fprintf(fp, "%s", "this is runoob.com");
   fclose(fp);
   
   ret = remove(filename1);

   if(ret == 0) 
   {
      printf("remove file1 success!\n");
   }
   else 
   {
      printf("error,can not remove file1!\n");
   }

   ret = remove(filename2);
   if(ret == 0) 
   {
      printf("remove file2 success!\n");
   }
   else 
   {
      printf("error,can not remove file2!\n");
   }
}

static void malloc_and_free(void)
{
  int *p;
  int len;
  for(int i = 0; i < 100; i++)
  {
    len = 1024*(i+1);
    p = malloc(len);
    if(p)
    {
      printf("malloc %d bytes success!\n",len);
      free(p);
    }
  }
}
void Testmusl(void)
{
  printf("--------start test file read and write!--------\n");
  file_read_and_write();
  printf("----------start test file operationsn!---------\n");
  file_operations();
  printf("---------start test malloc and free!-----------\n");
  malloc_and_free();
}
#endif
#endif
