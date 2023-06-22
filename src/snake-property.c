#include "snake-property.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

SnakeRecUnit*
snake_rec_unit_new ()
{
        SnakeRecUnit* rec;
        rec = OBJECT_NEW (SnakeRecUnit);
        snake_rec_unit_init (rec);
        return rec;
}

void
snake_rec_unit_destory (SnakeRecUnit* rec)
{
        if (rec == NULL)
                return;
        free (rec);
}

void
snake_rec_unit_init (SnakeRecUnit* rec)
{
        if (rec == NULL)
                return;
        rec->create_time  = 0L;
        rec->next         = NULL;
        rec->snake_length = 0;
}

/*链销毁器*/
void
snake_rec_unit_destory_chain_all (SnakeRecUnit* head)
{
        SnakeRecUnit *rec, *tmp;
        rec = tmp = head;
        while (rec != NULL) {
                tmp = rec->next;
                snake_rec_unit_destory (rec);
                rec = tmp;
        }
}

/*创建一个贪吃蛇长度记录对象*/
SnakeRecUnit*
snake_rec_unit_create (uint lenth, ulong _s_time)
{
        SnakeRecUnit* rec;
        rec               = snake_rec_unit_new ();
        rec->create_time  = _s_time;
        rec->snake_length = lenth;
        return rec;
}

SnakeRecorder*
snake_recorder_new ()
{
        SnakeRecorder* cder;
        cder = OBJECT_NEW (SnakeRecorder);
        snake_recorder_init (cder);
        return cder;
}

void
snake_recorder_destory (SnakeRecorder* cder)
{
        if (cder == NULL)
                return;
        snake_recorder_unset (cder);
        free (cder);
        return;
}

void
snake_recorder_init (SnakeRecorder* cder)
{
        if (cder == NULL)
                return;
        cder->prty_version = SNAKE_PRTY_VERSION;
        cder->record_num   = 0;
        cder->rec_header   = NULL;
        cder->rec_end      = NULL;
        cder->rec_iter     = NULL;
}

void
snake_recorder_unset (SnakeRecorder* cder)
{
        if (cder == NULL)
                return;
        snake_rec_unit_destory_chain_all (cder->rec_header);
}

/*向记录器新增一条记录*/
void
snake_recorder_add_rec_unit (SnakeRecorder* recorder, SnakeRecUnit* unit)
{
        if (!recorder || !unit)
                return;
        recorder->record_num += 1;
        if (recorder->rec_header == NULL) {
                recorder->rec_header = unit;
                recorder->rec_end    = unit;
                return;
        }
        recorder->rec_end->next = unit;
        recorder->rec_end       = unit;
}

void
snake_recorder_pull_from_bdata (SnakeRecorder* recorder, void* bdata)
{
        int           record_num;
        uint          snake_length;
        ulong         create_time;
        SnakeRecUnit* rec_tmp;
        void*         buf;
        if (!recorder || !bdata)
                return;
        buf        = bdata;
        record_num = (int)((ulong)(*(char**)buf) ^ SNAKE_PRTY_HEAD);
        buf += sizeof (void*);
        for (int i = 0; i < record_num; ++i) {
                snake_length = (uint)(*(uint*)buf);
                buf += sizeof (uint);
                create_time = (ulong)(*(ulong*)buf);
                buf += sizeof (ulong);
                rec_tmp = snake_rec_unit_create (snake_length, create_time);
                snake_recorder_add_rec_unit (recorder, rec_tmp);
        }
}

/*迭代一条记录*/
SnakeRecUnit*
snake_recorder_iter_get (SnakeRecorder* recorder)
{
        static SnakeRecUnit* g_rec_iter;
        if (recorder == NULL || recorder->rec_iter == NULL)
                return NULL;
        g_rec_iter         = recorder->rec_iter;
        recorder->rec_iter = recorder->rec_iter->next;
        return g_rec_iter;
}

/*重设迭代器*/
void
snake_recorder_iter_reset (SnakeRecorder* recorder)
{
        if (recorder == NULL)
                return;
        recorder->rec_iter = recorder->rec_header;
}

/*二进制数据更新器*/
void
snake_recorder_update (SnakePrtyFile* prtyf_obj, void* snake_recorder_obj)
{
        int            will_length;
        void*          _bdata_ptr = NULL;
        ulong          record_num;
        SnakeRecUnit*  _unit;
        SnakeRecorder* cder = (SnakeRecorder*)snake_recorder_obj;
        if (!prtyf_obj || !cder)
                return;
        /*先释放原来的二进制数据*/
        if (prtyf_obj->prty_bdata)
                free (prtyf_obj->prty_bdata);
        /*从数据体长度计算出需要的二进制文件大小*/
        will_length = cder->record_num * (sizeof (int) + sizeof (ulong)) + 16;
        prtyf_obj->bdata_length = will_length;
        /*根据文件大小分配出空间*/
        prtyf_obj->prty_bdata = malloc (will_length);
        _bdata_ptr            = prtyf_obj->prty_bdata;
        /*空间置零*/
        memset (_bdata_ptr, 0, will_length);
        /*文件头部八字节为魔数头部和数据结构个数*/
        record_num = SNAKE_PRTY_HEAD | cder->record_num;
        memcpy (_bdata_ptr, (void*)&record_num, sizeof (ulong));

        _bdata_ptr += sizeof (void*);
        /*重置、初始化迭代器*/
        snake_recorder_iter_reset (cder);
        /*循环拷贝数据结构*/
        for (int i = 0; i < cder->record_num; ++i) {
                /*获取到第一个元素*/
                _unit = snake_recorder_iter_get (cder);
                if (!_unit)
                        break;
                /*拷贝蛇长数据*/
                memcpy (_bdata_ptr, &_unit->snake_length, sizeof (uint));
                _bdata_ptr += sizeof (uint);
                /*拷贝时间信息*/
                memcpy (_bdata_ptr, &_unit->create_time, sizeof (ulong));
                _bdata_ptr += sizeof (ulong);
        }
}

SnakePrtyFile*
snake_prty_file_new ()
{
        SnakePrtyFile* pf;
        pf = OBJECT_NEW (SnakePrtyFile);
        snake_prty_file_init (pf);
        return pf;
}

void
snake_prty_file_destory (SnakePrtyFile* pf)
{
        if (pf == NULL)
                return;
        snake_prty_file_unset (pf);
        free (pf);
}

void
snake_prty_file_init (SnakePrtyFile* pf)
{
        if (pf == NULL)
                return;
        pf->any_object          = NULL;
        pf->bdata_length        = 0;
        pf->prty_bdata          = NULL;
        pf->prty_file_path_name = NULL;
        pf->prty_update_func    = NULL;
        pf->next                = NULL;
}

void
snake_prty_file_unset (SnakePrtyFile* pf)
{
        if (pf == NULL)
                return;
        snake_prty_file_bdata_write (pf);
        if (pf->prty_bdata)
                free (pf->prty_bdata);
        if (pf->prty_file_path_name)
                free (pf->prty_file_path_name);
}

/*设置外部数据结构*/
void
snake_prty_file_set_dataobject (SnakePrtyFile* prtyf_obj, void* any_obj)
{
        if (!prtyf_obj || !any_obj)
                return;
        prtyf_obj->any_object = any_obj;
}

/*设置外部数据更新函数*/
void
snake_prty_file_set_update_func (SnakePrtyFile* prtyf_obj,
                                 void (*update_fn) (SnakePrtyFile*, void*))
{
        if (!prtyf_obj || !update_fn)
                return;
        prtyf_obj->prty_update_func = update_fn;
}

/*为配置文件对象设置文件路径*/
void
snake_prty_file_set_file_path_name (SnakePrtyFile* prtyf_obj,
                                    const char*    fp_name)
{
        int _s_len;
        if (!prtyf_obj || !fp_name)
                return;

        if (prtyf_obj->prty_file_path_name)
                free (prtyf_obj->prty_file_path_name);
        _s_len                         = strnlen (fp_name, 1024) + 1;
        prtyf_obj->prty_file_path_name = (char*)malloc (_s_len);
        strncpy (prtyf_obj->prty_file_path_name, fp_name, _s_len);
}

/*更新二进制数据*/
void
snake_prty_file_update (SnakePrtyFile* prtyf_obj)
{
        if (!prtyf_obj || !prtyf_obj->any_object ||
            !prtyf_obj->prty_update_func)
                return;
        prtyf_obj->prty_update_func (prtyf_obj, prtyf_obj->any_object);
}

/*从文件中拉取配置文件内容*/
void
snake_prty_file_bdata_read (SnakePrtyFile* prtyf_obj)
{
        FILE* fptr;
        if (!prtyf_obj || !prtyf_obj->prty_file_path_name)
                return;

        fptr = fopen (prtyf_obj->prty_file_path_name, "r");
        fprintf (stdout,
                 "snake_prty_file_bdata_read: 打开 <%s>\n",
                 prtyf_obj->prty_file_path_name);
        if (fptr == NULL) {
                /*若此前无配置文件，则创建新配置文件，且不拉取任何内容*/
                fptr = fopen (prtyf_obj->prty_file_path_name, "w");
                if (fptr == NULL) {
                        fprintf (stderr,
                                 "配置文件 <%s> 路径有误！\n",
                                 prtyf_obj->prty_file_path_name);
                        exit (1);
                }
                fclose (fptr);
                return;
        }
        if (fseek (fptr, 0, SEEK_END) == -1) {
                perror ("snake_prty_file_bdata_read, fseek:");
                exit (1);
        }
        prtyf_obj->bdata_length = (int)ftell (fptr) + 1;
        rewind (fptr);
        if (prtyf_obj->prty_bdata) {
                free (prtyf_obj->prty_bdata);
                prtyf_obj->prty_bdata = NULL;
        }
        if (prtyf_obj->bdata_length > 1) {
                prtyf_obj->prty_bdata = malloc (prtyf_obj->bdata_length);
                fread (prtyf_obj->prty_bdata, prtyf_obj->bdata_length, 1, fptr);
        }
        fclose (fptr);
}

/*将配置文件写入已指定的文件*/
void
snake_prty_file_bdata_write (SnakePrtyFile* prtyf_obj)
{
        FILE* fptr;
        if (!prtyf_obj || !prtyf_obj->prty_file_path_name)
                return;
        fptr = fopen (prtyf_obj->prty_file_path_name, "w");
        if (fptr == NULL) {
                perror ("snake_prty_file_bdata_write, fopen:");
                fprintf (stderr,
                         "fopen <%s> 时出错！\n",
                         prtyf_obj->prty_file_path_name);
                exit (1);
        }
        snake_prty_file_update (prtyf_obj);
        if (prtyf_obj->bdata_length != 0)
                fwrite (prtyf_obj->prty_bdata,
                        prtyf_obj->bdata_length,
                        1,
                        fptr);
        fclose (fptr);
}

SnakeProperty*
snake_property_new ()
{
        SnakeProperty* prty;
        prty = OBJECT_NEW (SnakeProperty);
        snake_property_init (prty);
        return prty;
}

void
snake_property_destory (SnakeProperty* prty)
{
        if (prty == NULL)
                return;
        snake_property_unset (prty);
        free (prty);
        return;
}

void
snake_property_init (SnakeProperty* prty)
{
        if (prty == NULL)
                return;
        /*初始化时即获取 HOME 环境变量*/
        prty->user_home_path_name = getenv ("HOME");
        prty->app_path_name       = NULL;
}

void
snake_property_unset (SnakeProperty* prty)
{
        SnakePrtyFile* tmp;
        if (prty == NULL)
                return;
        if (prty->app_path_name)
                free (prty->app_path_name);
        prty->app_path_name = NULL;
        /*释放配置文件对象*/
        tmp = prty->head;
        while (tmp != NULL) {
                snake_prty_file_destory (tmp);
                tmp = tmp->next;
        }
}

void
snake_property_set_app_path (SnakeProperty* prty, const char* app_path_name)
{
        int   uhpath_len, apath_len;
        char* cptr;
        if (!prty || !app_path_name || !prty->user_home_path_name)
                return;
        snake_property_unset (prty);
        uhpath_len = strnlen (prty->user_home_path_name, 1024);
        apath_len  = strnlen (app_path_name, 1024);
        /*分配完整路径大小空间*/
        cptr = prty->app_path_name = (char*)malloc (uhpath_len + apath_len + 2);
        strcpy (cptr, prty->user_home_path_name);
        cptr += uhpath_len;
        /*绝勿使用 strncpy ！！！如果指定长度 len 超过 array
         * 空间大小，会覆盖后方空间导致溢出*/
        strcpy (cptr++, "/");
        strcpy (cptr, app_path_name);
}

void
snake_property_load_prty (SnakeProperty* prty)
{
        int            app_path_name_len;
        static char    buffer[1024];
        char*          cptr;
        DIR*           _prty_dr;
        struct dirent* _dp;
        if (!prty || !prty->app_path_name)
                return;
re_open:
        _prty_dr = opendir (prty->app_path_name);
        if (_prty_dr == NULL) {
                fprintf (stdout, "将在 <%s> 创建目录\n", prty->app_path_name);
                if (mkdir (prty->app_path_name, 0700) == -1) {
                        perror ("snake_property_load_prty:(mkdir)");
                        exit (1);
                }
                goto re_open;
        }
        app_path_name_len = strnlen (prty->app_path_name, 1024);
        while (1) {
                _dp = readdir (_prty_dr);
                if (!_dp)
                        break;
                /*检测文件是否为 . 和 ..*/
                if (_dp->d_name[0] == '.' && (_dp->d_name[1] == '\0' || _dp->d_name[1] == '.'))
                        continue;
                memset (buffer, 0, 1024);
                cptr = buffer;
                if (prty->head == NULL) {
                        prty->head = prty->end = snake_prty_file_new ();
                } else {
                        prty->end->next = snake_prty_file_new ();
                        prty->end       = prty->end->next;
                }
                strcpy (cptr, prty->app_path_name);
                cptr += app_path_name_len;
                strcpy (cptr++, "/");
                strcpy (cptr, _dp->d_name);
                snake_prty_file_set_file_path_name (prty->end, buffer);
        }
        closedir (_prty_dr);
}

SnakePrtyFile*
snake_property_get_prty_from_name (SnakeProperty* prty, const char* prty_name)
{
        static char    buffer[1024];
        char*          cptr;
        SnakePrtyFile* tmp;

        if (!prty || !prty_name)
                return NULL;

        cptr = buffer;
        strcpy (cptr, prty->app_path_name);
        cptr += strlen (prty->app_path_name);
        strcpy (cptr++, "/");
        strcpy (cptr, prty_name);

        tmp = prty->head;
        while (tmp != NULL) {
                if (strcmp (buffer, tmp->prty_file_path_name) == 0)
                        return tmp;
                tmp = tmp->next;
        }
        return NULL;
}

SnakePrtyFile*
snake_property_add_prty_file (SnakeProperty* prty, const char* prty_name)
{
        static char    buffer[1024];
        char*          cptr;
        SnakePrtyFile* tmp;
        if (!prty || !prty_name)
                return NULL;
        tmp = snake_property_get_prty_from_name (prty, prty_name);
        if (tmp)
                return tmp;
        cptr = buffer;
        strcpy (cptr, prty->app_path_name);
        cptr += strlen (prty->app_path_name);
        strcpy (cptr++, "/");
        strcpy (cptr, prty_name);
        tmp = snake_prty_file_new ();
        snake_prty_file_set_file_path_name (tmp, buffer);
        snake_prty_file_bdata_write (tmp);

        prty->end->next = tmp;
        prty->end       = tmp;
        return tmp;
}