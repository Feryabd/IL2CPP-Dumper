#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined(__GNUC__) || defined(__clang__)
    #define ALIGN_TYPE(val) __attribute__((aligned(val)))
#elif defined(_MSC_VER)
    #define ALIGN_TYPE(val) alignas(val)
#else
    #define ALIGN_TYPE(size)
#endif

#define IL2CPP_ZERO_LEN_ARRAY 0

typedef int32_t TypeIndex;
typedef int32_t TypeDefinitionIndex;
typedef int32_t FieldIndex;
typedef int32_t DefaultValueIndex;
typedef int32_t DefaultValueDataIndex;
typedef int32_t CustomAttributeIndex;
typedef int32_t ParameterIndex;
typedef int32_t MethodIndex;
typedef int32_t GenericMethodIndex;
typedef int32_t PropertyIndex;
typedef int32_t EventIndex;
typedef int32_t GenericContainerIndex;
typedef int32_t GenericParameterIndex;
typedef int16_t GenericParameterConstraintIndex;
typedef int32_t NestedTypeIndex;
typedef int32_t InterfacesIndex;
typedef int32_t VTableIndex;
typedef int32_t RGCTXIndex;
typedef int32_t StringIndex;
typedef int32_t StringLiteralIndex;
typedef int32_t GenericInstIndex;
typedef int32_t ImageIndex;
typedef int32_t AssemblyIndex;
typedef int32_t InteropDataIndex;

typedef const struct ___Il2CppMetadataImageHandle* Il2CppMetadataImageHandle;
typedef const struct ___Il2CppMetadataCustomAttributeHandle* Il2CppMetadataCustomAttributeHandle;
typedef const struct ___Il2CppMetadataTypeHandle* Il2CppMetadataTypeHandle;
typedef const struct ___Il2CppMetadataMethodHandle* Il2CppMetadataMethodDefinitionHandle;
typedef const struct ___Il2CppMetadataGenericContainerHandle* Il2CppMetadataGenericContainerHandle;
typedef const struct ___Il2CppMetadataGenericParameterHandle* Il2CppMetadataGenericParameterHandle;

typedef enum Il2CppTypeEnum
{
    IL2CPP_TYPE_END        = 0x00,
    IL2CPP_TYPE_VOID       = 0x01,
    IL2CPP_TYPE_BOOLEAN    = 0x02,
    IL2CPP_TYPE_CHAR       = 0x03,
    IL2CPP_TYPE_I1         = 0x04,
    IL2CPP_TYPE_U1         = 0x05,
    IL2CPP_TYPE_I2         = 0x06,
    IL2CPP_TYPE_U2         = 0x07,
    IL2CPP_TYPE_I4         = 0x08,
    IL2CPP_TYPE_U4         = 0x09,
    IL2CPP_TYPE_I8         = 0x0a,
    IL2CPP_TYPE_U8         = 0x0b,
    IL2CPP_TYPE_R4         = 0x0c,
    IL2CPP_TYPE_R8         = 0x0d,
    IL2CPP_TYPE_STRING     = 0x0e,
    IL2CPP_TYPE_PTR        = 0x0f,
    IL2CPP_TYPE_BYREF      = 0x10,
    IL2CPP_TYPE_VALUETYPE  = 0x11,
    IL2CPP_TYPE_CLASS      = 0x12,
    IL2CPP_TYPE_VAR        = 0x13,
    IL2CPP_TYPE_ARRAY      = 0x14,
    IL2CPP_TYPE_GENERICINST = 0x15,
    IL2CPP_TYPE_TYPEDBYREF = 0x16,
    IL2CPP_TYPE_I          = 0x18,
    IL2CPP_TYPE_U          = 0x19,
    IL2CPP_TYPE_FNPTR      = 0x1b,
    IL2CPP_TYPE_OBJECT     = 0x1c,
    IL2CPP_TYPE_SZARRAY    = 0x1d,
    IL2CPP_TYPE_MVAR       = 0x1e,
    IL2CPP_TYPE_CMOD_REQD  = 0x1f,
    IL2CPP_TYPE_CMOD_OPT   = 0x20,
    IL2CPP_TYPE_INTERNAL   = 0x21,
    IL2CPP_TYPE_MODIFIER   = 0x40,
    IL2CPP_TYPE_SENTINEL   = 0x41,
    IL2CPP_TYPE_PINNED     = 0x45,
    IL2CPP_TYPE_ENUM       = 0x55,
    IL2CPP_TYPE_IL2CPP_TYPE_INDEX = 0xff
} Il2CppTypeEnum;

typedef struct Il2CppClass Il2CppClass;
typedef struct Il2CppImage Il2CppImage;
typedef struct Il2CppAssembly Il2CppAssembly;
typedef struct Il2CppDomain Il2CppDomain;
typedef struct Il2CppAppDomain Il2CppAppDomain;
typedef struct Il2CppAppDomainSetup Il2CppAppDomainSetup;
typedef struct Il2CppAppContext Il2CppAppContext;
typedef struct Il2CppObject Il2CppObject;
typedef struct Il2CppThread Il2CppThread;
typedef struct MethodInfo MethodInfo;
typedef struct FieldInfo FieldInfo;
typedef struct PropertyInfo PropertyInfo;
typedef struct Il2CppGenericClass Il2CppGenericClass;
typedef struct Il2CppGenericInst Il2CppGenericInst;
typedef struct Il2CppGenericMethod Il2CppGenericMethod;
typedef struct Il2CppGenericContainer Il2CppGenericContainer;
typedef struct Il2CppType Il2CppType;
typedef struct Il2CppArrayType Il2CppArrayType;
typedef struct Il2CppInteropData Il2CppInteropData;

typedef void (*Il2CppMethodPointer)();

typedef struct Il2CppArrayType
{
    const Il2CppType* etype;
    uint8_t rank;
    uint8_t numsizes;
    uint8_t numlobounds;
    int *sizes;
    int *lobounds;
} Il2CppArrayType;

typedef struct Il2CppType
{
    union
    {
        void* dummy;
        TypeDefinitionIndex __klassIndex;
        Il2CppMetadataTypeHandle typeHandle;
        const Il2CppType *type;
        Il2CppArrayType *array;
        GenericParameterIndex __genericParameterIndex;
        Il2CppMetadataGenericParameterHandle genericParameterHandle;
        Il2CppGenericClass *generic_class;
    } data;
    unsigned int attrs    : 16;
    Il2CppTypeEnum type     : 8;
    unsigned int num_mods : 5;
    unsigned int byref    : 1;
    unsigned int pinned   : 1;
    unsigned int valuetype : 1;
} Il2CppType;

typedef struct FieldInfo
{
    const char* name;
    const Il2CppType* type;
    Il2CppClass *parent;
    int32_t offset;
    uint32_t token;
} FieldInfo;

typedef struct PropertyInfo
{
    Il2CppClass *parent;
    const char *name;
    const MethodInfo *get;
    const MethodInfo *set;
    uint32_t attrs;
    uint32_t token;
} PropertyInfo;

typedef struct MethodInfo
{
    Il2CppMethodPointer methodPointer;
    Il2CppMethodPointer virtualMethodPointer;
    void* invoker_method;
    const char* name;
    Il2CppClass *klass;
    const Il2CppType *return_type;
    const Il2CppType** parameters;

    union
    {
        const void* rgctx_data;
        Il2CppMetadataMethodDefinitionHandle methodMetadataHandle;
    };

    union
    {
        const Il2CppGenericMethod* genericMethod;
        Il2CppMetadataGenericContainerHandle genericContainerHandle;
    };

    uint32_t token;
    uint16_t flags;
    uint16_t iflags;
    uint16_t slot;
    uint8_t parameters_count;
    uint8_t is_generic : 1;
    uint8_t is_inflated : 1;
    uint8_t wrapper_type : 1;
    uint8_t has_full_generic_sharing_signature : 1;
    uint8_t is_unmanaged_callers_only : 1;
} MethodInfo;

typedef struct VirtualInvokeData
{
    Il2CppMethodPointer methodPtr;
    const MethodInfo* method;
} VirtualInvokeData;

typedef struct Il2CppClass
{
    const Il2CppImage* image;
    void* gc_desc;
    const char* name;
    const char* namespaze;
    Il2CppType byval_arg;
    Il2CppType this_arg;
    Il2CppClass* element_class;
    Il2CppClass* castClass;
    Il2CppClass* declaringType;
    Il2CppClass* parent;
    Il2CppGenericClass *generic_class;
    Il2CppMetadataTypeHandle typeMetadataHandle;
    const void* interopData;
    Il2CppClass* klass;
    FieldInfo* fields;
    const void* events;
    const PropertyInfo* properties;
    const MethodInfo** methods;
    Il2CppClass** nestedTypes;
    Il2CppClass** implementedInterfaces;
    void* interfaceOffsets;
    void* static_fields;
    union
    {
        const void* rgctx_data;
        const void* genericParameterFlags;
    };
    Il2CppClass** typeHierarchy;
    void *unity_user_data;
    uintptr_t initializationExceptionGCHandle;
    uint32_t cctor_started;
    uint32_t cctor_finished_or_no_cctor;
    ALIGN_TYPE(8) size_t cctor_thread;
    Il2CppMetadataGenericContainerHandle genericContainerHandle;
    uint32_t instance_size;
    uint32_t stack_slot_size;
    uint32_t actualSize;
    uint32_t element_size;
    int32_t native_size;
    uint32_t static_fields_size;
    uint32_t thread_static_fields_size;
    int32_t thread_static_fields_offset;
    uint32_t flags;
    uint32_t token;
    uint16_t method_count;
    uint16_t property_count;
    uint16_t field_count;
    uint16_t event_count;
    uint16_t nested_type_count;
    uint16_t vtable_count;
    uint16_t interfaces_count;
    uint16_t interface_offsets_count;
    uint8_t typeHierarchyDepth;
    uint8_t genericRecursionDepth;
    uint8_t rank;
    uint8_t minimumAlignment;
    uint8_t packingSize;
    uint8_t initialized_and_no_error : 1;
    uint8_t initialized : 1;
    uint8_t enumtype : 1;
    uint8_t nullabletype : 1;
    uint8_t is_generic : 1;
    uint8_t has_references : 1;
    uint8_t init_pending : 1;
    uint8_t size_init_pending : 1;
    uint8_t size_inited : 1;
    uint8_t has_finalize : 1;
    uint8_t has_cctor : 1;
    uint8_t is_blittable : 1;
    uint8_t is_import_or_windows_runtime : 1;
    uint8_t is_vtable_initialized : 1;
    uint8_t is_byref_like : 1;
    VirtualInvokeData vtable[IL2CPP_ZERO_LEN_ARRAY];
} Il2CppClass;

typedef struct Il2CppDomain
{
    Il2CppAppDomain* domain;
    Il2CppAppDomainSetup* setup;
    Il2CppAppContext* default_context;
    Il2CppObject* ephemeron_tombstone;
    const char* friendly_name;
    uint32_t domain_id;
    volatile int threadpool_jobs;
    void* agent_info;
} Il2CppDomain;

typedef struct Il2CppAssemblyName
{
    const char* name;
    const char* culture;
    const uint8_t* public_key;
    uint32_t hash_alg;
    int32_t hash_len;
    uint32_t flags;
    int32_t major;
    int32_t minor;
    int32_t build;
    int32_t revision;
    uint8_t public_key_token[8];
} Il2CppAssemblyName;

typedef struct Il2CppImage
{
    const char* name;
    const char *nameNoExt;
    Il2CppAssembly* assembly;
    uint32_t typeCount;
    uint32_t exportedTypeCount;
    uint32_t customAttributeCount;
    Il2CppMetadataImageHandle metadataHandle;
    void* nameToClassHashTable;
    const void* codeGenModule;
    uint32_t token;
    uint8_t dynamic;
} Il2CppImage;

typedef struct Il2CppAssembly
{
    Il2CppImage* image;
    uint32_t token;
    int32_t referencedAssemblyStart;
    int32_t referencedAssemblyCount;
    Il2CppAssemblyName aname;
} Il2CppAssembly;

#define FIELD_ATTRIBUTE_FIELD_ACCESS_MASK     0x0007
#define FIELD_ATTRIBUTE_PRIVATE               0x0001
#define FIELD_ATTRIBUTE_FAM_AND_ASSEM         0x0002
#define FIELD_ATTRIBUTE_ASSEMBLY              0x0003
#define FIELD_ATTRIBUTE_FAMILY                0x0004
#define FIELD_ATTRIBUTE_FAM_OR_ASSEM          0x0005
#define FIELD_ATTRIBUTE_PUBLIC                0x0006
#define FIELD_ATTRIBUTE_STATIC                0x0010
#define FIELD_ATTRIBUTE_INIT_ONLY             0x0020
#define FIELD_ATTRIBUTE_LITERAL               0x0040

#define METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK        0x0007
#define METHOD_ATTRIBUTE_PRIVATE                   0x0001
#define METHOD_ATTRIBUTE_FAM_AND_ASSEM             0x0002
#define METHOD_ATTRIBUTE_ASSEM                     0x0003
#define METHOD_ATTRIBUTE_FAMILY                    0x0004
#define METHOD_ATTRIBUTE_FAM_OR_ASSEM              0x0005
#define METHOD_ATTRIBUTE_PUBLIC                    0x0006
#define METHOD_ATTRIBUTE_STATIC                    0x0010
#define METHOD_ATTRIBUTE_FINAL                     0x0020
#define METHOD_ATTRIBUTE_VIRTUAL                   0x0040
#define METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK        0x0100
#define METHOD_ATTRIBUTE_REUSE_SLOT                0x0000
#define METHOD_ATTRIBUTE_NEW_SLOT                  0x0100
#define METHOD_ATTRIBUTE_ABSTRACT                  0x0400
#define METHOD_ATTRIBUTE_PINVOKE_IMPL              0x2000

#define TYPE_ATTRIBUTE_VISIBILITY_MASK       0x00000007
#define TYPE_ATTRIBUTE_NOT_PUBLIC            0x00000000
#define TYPE_ATTRIBUTE_PUBLIC                0x00000001
#define TYPE_ATTRIBUTE_NESTED_PUBLIC         0x00000002
#define TYPE_ATTRIBUTE_NESTED_PRIVATE        0x00000003
#define TYPE_ATTRIBUTE_NESTED_FAMILY         0x00000004
#define TYPE_ATTRIBUTE_NESTED_ASSEMBLY       0x00000005
#define TYPE_ATTRIBUTE_NESTED_FAM_AND_ASSEM  0x00000006
#define TYPE_ATTRIBUTE_NESTED_FAM_OR_ASSEM   0x00000007
#define TYPE_ATTRIBUTE_INTERFACE             0x00000020
#define TYPE_ATTRIBUTE_ABSTRACT              0x00000080
#define TYPE_ATTRIBUTE_SEALED                0x00000100

#define PARAM_ATTRIBUTE_IN                 0x0001
#define PARAM_ATTRIBUTE_OUT                0x0002
