#ifndef MANAGER_H_
#define MANAGER_H_

#include "../ulti/support.h"
#include "file_manager.h"
#include "process_manager.h"

namespace manager {
	inline ProcessManager kProcMan;
	void Init();
}
#endif  // MANAGER_H_
