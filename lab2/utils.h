// File name: "EnumToString.h"
#undef DECL_ENUM_ELEMENT
#undef BEGIN_ENUM
#undef END_ENUM

#ifndef GENERATE_ENUM_STRINGS
#define DECL_ENUM_ELEMENT(element) element
#define BEGIN_ENUM(ENUM_NAME) typedef enum tag##ENUM_NAME
#define END_ENUM(ENUM_NAME) \
    ENUM_NAME;              \
    char *GetString##ENUM_NAME(enum tag##ENUM_NAME index);
#else
#define DECL_ENUM_ELEMENT(element) #element
#define BEGIN_ENUM(ENUM_NAME) char *gs_##ENUM_NAME[] =
#define END_ENUM(ENUM_NAME)                               \
    ;                                                     \
    char *GetString##ENUM_NAME(enum tag##ENUM_NAME index) \
    {                                                     \
        return gs_##ENUM_NAME[index];                     \
    }
#endif

#ifndef EVENTTYPE_TO_STRING
#define EVENTTYPE_TO_STRING
typedef enum EventType
{
    TRANS_TO_READY,
    TRANS_TO_PREEMPT,
    TRANS_TO_RUN,
    TRANS_TO_BLOCK
} EventType;
inline const char *EventToString(EventType v)
{
    switch (v)
    {
    case TRANS_TO_READY:
        return "READY";
    case TRANS_TO_PREEMPT:
        return "PREEMPT";
    case TRANS_TO_RUN:
        return "RUN";
    case TRANS_TO_BLOCK:
        return "BLOCK";
    default:
        return "UNKOWN";
    }
}
#endif
#ifndef PROCSTATE_TO_STRING
#define PROCSTATE_TO_STRING
typedef enum ProcState
{
    RUNNG,
    READY,
    BLOCKED,
    CREATED
} ProcState;

inline const char *ProcToString(ProcState v)
{
    switch (v)
    {
    case RUNNG:
        return "RUNNG";
    case READY:
        return "READY";
    case BLOCKED:
        return "BLOCK";
    case CREATED:
        return "CREATED";
    default:
        return "UNKOWN";
    }
}
#endif


