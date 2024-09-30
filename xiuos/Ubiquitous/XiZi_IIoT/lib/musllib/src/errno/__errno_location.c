#include <errno.h>
#include "pthread_impl.h"
#include <xs_ktask.h>

int *__errno_location(void)
{
	return KObtainExstatus();
}

weak_alias(__errno_location, ___errno_location);
