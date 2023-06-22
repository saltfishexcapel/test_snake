#include "snake-application.h"
#include "snake-internal.h"
#include "snake-property.h"
#include "snake_engine_v1.h"

SnakeProperty* global_prty;
SnakeRecorder* global_recorder;
SnakePrtyFile* global_prtyf_obj;

int
main (int argc, char* argv[])
{
        int               ret;
        SnakeApplication* app;

        global_prty = snake_property_new ();
        snake_property_set_app_path (global_prty, ".config/test_snake");
        snake_property_load_prty (global_prty);

        global_recorder = snake_recorder_new ();

        /*新建或设定配置文件对象*/
        global_prtyf_obj =
                snake_property_add_prty_file (global_prty, "statistics");
        /*读取配置文件到配置文件对象内存*/
        snake_prty_file_bdata_read (global_prtyf_obj);
        /*设置贪吃蛇记录对象更新方法*/
        snake_prty_file_set_update_func (global_prtyf_obj,
                                         snake_recorder_update);
        /*设置贪吃蛇记录对象*/
        snake_prty_file_set_dataobject (global_prtyf_obj, global_recorder);
        /*从二进制文件中拉取全部记录*/
        snake_recorder_pull_from_bdata (global_recorder,
                                        global_prtyf_obj->prty_bdata);

        app = snake_application_new ("org.example.snake",
                                     G_APPLICATION_FLAGS_NONE);
        ret = g_application_run (G_APPLICATION (app), argc, argv);

        snake_map_destory (g_map);
        snake_property_destory (global_prty);
        snake_recorder_destory (global_recorder);

        return ret;
}