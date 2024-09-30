#include <rtthread.h>

#if defined RT_USING_DFS &&  defined RT_USING_DFS_RAMFS
#include <dfs_fs.h>
#include "dfs_ramfs.h"

int mnt_ramfs_init(void)
{
    if (dfs_mount(RT_NULL, "/", "ram", 0, dfs_ramfs_create(rt_malloc(1024),1024)) == 0)
    {
        rt_kprintf("RAM file system initializated!\n");
    }
    else
    {
        rt_kprintf("RAM file system initializate failed!\n");
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_ramfs_init);

#endif