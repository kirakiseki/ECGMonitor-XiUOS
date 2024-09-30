#include "assert.h"
#include "xizi.h"

void __assert_fail(const char* expr, const char* file, int line, const char* func) {
  KPrintf("%s:%d: %s: assertion \"%s\" failed\n", file, line, func, expr);
  KTaskDescriptorType current = GetKTaskDescriptor();
  if (current)
  {
    KPrintf("Task:%-8.*s will be aborted!\n", NAME_NUM_MAX, current->task_base_info.name);
    /* pend current task */
    SuspendKTask(current->id.id);
    /* schedule */
    DO_KTASK_ASSIGN;
  }
  while (1);
}
