#ifndef OBJECT_TYPE_H
#define OBJECT_TYPE_H

#include <stdbool.h>

#ifndef NULL
        #define NULL ((void*)0)
#endif

/*空指针类型*/
typedef void* ptr;
/*无符号类型*/
typedef unsigned int  uint;
typedef unsigned short ushort;
typedef unsigned long  ulong;
/*RData 所支持的最大的数据长度*/
typedef unsigned long msize;
#define MAXDSIZE sizeof (msize)
/*RData 长数据结构*/
typedef unsigned long RData;

#define object_rdata_pack(any) ((RData)(any))
/*检查类型大小*/
#define RD_CHK(type) (sizeof (typedef) > MAXDSIZE ? 0 : 1)
/*将 RData 解包成对应类型数据*/
#define RD_UNPK(rdata, type) ((type)(rdata))

/* ******设置一对对象声明******
 *
 * 声明后需要按如下方式设置对象：
 * 类定义：
 * struct _ObjNameClass
 * {
 *      datas;
 * };
 *
 * 构造器定义：
 * ObjName*
 * obj_name_new ()
 * {
 *      constractor...
 * }
 *
 * 清除器定义：
 * void
 * obj_name_destory ()
 * {
 *      destoryer...
 * }
 *
 * ObjName: 主类型名称
 * obj_name: 别名（用于函数）
 *
 */
#define OBJECT_SET_TYPE(ObjName, obj_name)                              \
        typedef struct _##ObjName##Class ObjName##Class;                \
        typedef ObjName##Class           ObjName;                       \
        ObjName*                         obj_name##_new ();             \
        void                             obj_name##_destory (ObjName*); \
        void                             obj_name##_init (ObjName*);    \
        void                             obj_name##_unset (ObjName*);

#define OBJECT_SET_TYPE_RET_PARENT(ObjName, obj_name, ParentName)       \
        typedef struct _##ObjName##Class ObjName##Class;                \
        typedef ObjName##Class           ObjName;                       \
        ParentName*                      obj_name##_new ();             \
        void                             obj_name##_destory (ObjName*); \
        void                             obj_name##_init (ObjName*);    \
        void                             obj_name##_unset (ObjName*);

#define OBJECT_NEW(ObjectType) ((ObjectType*)malloc (sizeof (ObjectType)))

#endif