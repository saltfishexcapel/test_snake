#ifndef SNAKE_PROPERTY_H
#define SNAKE_PROPERTY_H

#include "object-type.h"

#define SNAKE_PRTY_VERSION 0x01
#define SNAKE_PRTY_HEAD    0x0000000000000000

OBJECT_SET_TYPE (SnakeRecUnit, snake_rec_unit)
OBJECT_SET_TYPE (SnakeRecorder, snake_recorder)
OBJECT_SET_TYPE (SnakePrtyFile, snake_prty_file)
OBJECT_SET_TYPE (SnakeProperty, snake_property)

extern SnakeProperty* global_prty;
extern SnakePrtyFile* global_prtyf_obj;
extern SnakeRecorder* global_recorder;

/*长度记录细胞单元*/
struct _SnakeRecUnitClass
{
        uint          snake_length;
        ulong         create_time;
        SnakeRecUnit* next;
};

/*链销毁器*/
void snake_rec_unit_destory_chain_all (SnakeRecUnit* head);

/*创建一个贪吃蛇长度记录对象*/
SnakeRecUnit* snake_rec_unit_create (uint lenth, ulong _s_time);

/*记录器对象，贪吃蛇分数只用新增不用删除，队列结构*/
struct _SnakeRecorderClass
{
        int           prty_version; /*配置版本*/
        int           record_num;
        SnakeRecUnit* rec_header;
        SnakeRecUnit* rec_end;
        SnakeRecUnit* rec_iter;
};

/*向记录器新增一条记录*/
void snake_recorder_add_rec_unit (SnakeRecorder* recorder, SnakeRecUnit* unit);

/*初始化时从二进制数据中拉取所有结构*/
void snake_recorder_pull_from_bdata (SnakeRecorder* recorder, void* bdata);

/*迭代一条记录*/
SnakeRecUnit* snake_recorder_iter_get (SnakeRecorder* recorder);
/*重设迭代器*/
void snake_recorder_iter_reset (SnakeRecorder* recorder);

/*二进制数据更新器*/
void snake_recorder_update (SnakePrtyFile* prtyf_obj, void* snake_recorder_obj);

/*配置文件对象*/
struct _SnakePrtyFileClass
{
        char* prty_file_path_name; /*配置文件路径*/
        void* prty_bdata;          /*配置文件二进制数据*/
        void* any_object;          /*需要进行配置的数据结构对象*/
        void (*prty_update_func) (SnakePrtyFile* self,
                                  void* any_obj); /*更新二进制数据的外部函数*/
        int            bdata_length;
        SnakePrtyFile* next;
};

/*设置外部数据结构*/
void snake_prty_file_set_dataobject (SnakePrtyFile* prtyf_obj, void* any_obj);
/*设置外部数据更新函数*/
void snake_prty_file_set_update_func (SnakePrtyFile* prtyf_obj,
                                      void (*update_fn) (SnakePrtyFile*,
                                                         void*));
/*为配置文件对象设置文件路径*/
void snake_prty_file_set_file_path_name (SnakePrtyFile* prtyf_obj,
                                         const char*    fp_name);

/*更新二进制数据*/
void snake_prty_file_update (SnakePrtyFile* prtyf_obj);

/*从文件中拉取配置文件内容*/
void snake_prty_file_bdata_read (SnakePrtyFile* prtyf_obj);
/*将配置文件写入已指定的文件*/
void snake_prty_file_bdata_write (SnakePrtyFile* prtyf_obj);

/*一般用户配置*/
struct _SnakePropertyClass
{
        /*用户主目录，不可释放*/
        const char* user_home_path_name;
        /*应用程序配置目录*/
        char* app_path_name;
        /*配置文件节点*/
        SnakePrtyFile* head;
        SnakePrtyFile* end;
};

/*设置应用程序配置目录，只需相对用户目录路径*/
void snake_property_set_app_path (SnakeProperty* prty,
                                  const char*    app_path_name);
/*载入本目录下所有配置文件*/
void snake_property_load_prty (SnakeProperty* prty);
/*获取已载入的配置文件*/
SnakePrtyFile* snake_property_get_prty_from_name (SnakeProperty* prty,
                                                  const char*    prty_name);
/*在本目录下新添加一个配置文件*/
SnakePrtyFile* snake_property_add_prty_file (SnakeProperty* prty,
                                             const char*    prty_name);

#endif
