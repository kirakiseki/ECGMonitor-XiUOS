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
 * @file adapter_powerlink.c
 * @brief Implement the connection powerlink adapter function
 * @version 3.0
 * @author AIIT XUOS Lab
 * @date 2022.09.27
 */

#include <adapter.h>

#ifdef POWERLINK_MN
extern int OplkDemoMnConsole(int argc, char *argv[]);

PRIV_SHELL_CMD_FUNCTION(OplkDemoMnConsole, a openPOWERLINK MN sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif

#ifdef POWERLINK_CN
extern int OplkDemoCnConsole(int argc, char *argv[]);

PRIV_SHELL_CMD_FUNCTION(OplkDemoCnConsole, a openPOWERLINK CN sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif
