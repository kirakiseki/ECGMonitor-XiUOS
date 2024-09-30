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
* @file:    test_shell.c
* @brief:   a application of shell function
* @version: 2.0
* @author:  AIIT XUOS Lab
* @date:    2022/9/26
*/

#include <transform.h>

void TestShellMain(int argc, char *agrv[])
{
    printf("%dparameter(s)\r\n", argc);
    for (char i = 1; i < argc; i++) {
        printf("%s\r\n", agrv[i]);
    }
}
PRIV_SHELL_CMD_FUNCTION(TestShellMain, a shell main sample 1, PRIV_SHELL_CMD_MAIN_ATTR);

void TestShellMainVoid(void)
{
    printf("XiUOS test_shell_main_void\r\n");
}
PRIV_SHELL_CMD_FUNCTION(TestShellMainVoid, a shell main sample 2, PRIV_SHELL_CMD_MAIN_ATTR);

void TestShellFunc(int i, char ch, char *str)
{
    printf("input int: %d, char: %c, string: %s\r\n", i, ch, str);
}
PRIV_SHELL_CMD_FUNCTION(TestShellFunc, a shell func sample, PRIV_SHELL_CMD_FUNC_ATTR);
