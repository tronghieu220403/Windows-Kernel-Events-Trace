#pragma once

#include "../../std/string/string.h"
#include "../../std/map/map.h"
#include "../../std/sync/mutex.h"
#include "../../template/register.h"
#include "../../template/flt-ex.h"

#define THREAD_TERMINATE                 (0x0001)  
#define THREAD_SUSPEND_RESUME            (0x0002)  
#define THREAD_GET_CONTEXT               (0x0008)  
#define THREAD_SET_CONTEXT               (0x0010)  
#define THREAD_SET_INFORMATION           (0x0020)  
#define THREAD_SET_THREAD_TOKEN          (0x0080)
#define THREAD_IMPERSONATE               (0x0100)
#define THREAD_DIRECT_IMPERSONATION      (0x0200)
#define THREAD_SET_LIMITED_INFORMATION   (0x0400)
#define THREAD_RESUME                    (0x1000)

#define PROCESS_TERMINATE					(0x0001)  
#define PROCESS_CREATE_THREAD				(0x0002)  
#define PROCESS_SET_SESSIONID				(0x0004)  
#define PROCESS_VM_OPERATION				(0x0008)  
#define PROCESS_VM_READ						(0x0010)  
#define PROCESS_VM_WRITE					(0x0020)  
#define PROCESS_DUP_HANDLE					(0x0040)  
#define PROCESS_CREATE_PROCESS				(0x0080)  
#define PROCESS_SET_QUOTA					(0x0100)  
#define PROCESS_SET_INFORMATION				(0x0200)  
#define PROCESS_QUERY_INFORMATION			(0x0400)  
#define PROCESS_SUSPEND_RESUME				(0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION	(0x1000)  
#define PROCESS_SET_LIMITED_INFORMATION		(0x2000)  

namespace query
{
}
