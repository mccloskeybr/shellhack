#ifndef COMMON_STATUS
#define COMMON_STATUS

#if DEBUG
#define ASSERT(expression) if (!(expression)) { *(int*)0 = 0; }
#else
#define ASSERT(expression)
#endif

#if DEBUG
// allows input of _STATUS in debugger on assertion failure to see the val.
#define ASSERT_OK(expression) 					    \
		{ 						                          \
			const Status _STATUS = (expression); 	\
			ASSERT(_STATUS == OK) 			          \
		}
#else
#define ASSERT_OK(expression)
#endif

#define UNREACHABLE *(int*)0 = 0;

#define RETURN_IF_ERROR(expression) 				  \
		{ 						                            \
			const Status _STATUS = (expression);	  \
			if (_STATUS != OK) { return _STATUS; } 	\
		}

// TODO: come up with custom error codes.
// shouldn't have to be restricted to RPC codes.
// could try looking up other error code sets.
typedef enum Status {
	OK,
	INVALID_ARGUMENT,
	FAILED_PRECONDITION,
	RESOURCE_EXHAUSTED,
	INTERNAL,
	NOT_FOUND
} Status;

#endif
