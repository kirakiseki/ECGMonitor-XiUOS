#include "xizi.h"

void abort(void) {
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