/****************************************************************************
 * libs/libc-musl/netdb/lib_getservbyname.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/**
* @file lib_getservbyname.c
* @brief nuttx source code
*        https://github.com/apache/incubator-nuttx.git
* @version 10.3.0 
* @author AIIT XUOS Lab
* @date 2022-08-04
*/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <netdb.h>

#ifdef CONFIG_LIBC_NETDB

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: getservbyname
 ****************************************************************************/

FAR struct servent *getservbyname(FAR const char *name,
                                  FAR const char *proto)
{
  static struct servent ent;
  FAR struct servent *res;
  int ret;

  ret = getservbyname_r(name, proto, &ent, NULL, 0, &res);
  return (ret != OK) ? NULL : res;
}

#endif /* CONFIG_LIBC_NETDB */
