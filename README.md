# test_snake，贪吃蛇
---
## 使用 Gtk4 编写的小游戏
我是用的是 Gnome 文档中推荐的源码管理方法，使用了如下结构
* main: 主函数
* snake-application: GtkApplication 子类
* snake-window: GtkWindow 子类，管理所有窗口小部件
* snake-internal: 贪吃蛇的逻辑控制部分
* snake-property: 一个还算通用的配置文件存储对象，可以把程序的一些必用数据存到 $HOME/.config/app-data 中，比如贪吃蛇的以往分数就是通过这个存储对象来进行储存的（设计存储的时候整个人都麻了）
* snake_engine_v1: 一个通用的贪吃蛇引擎，包含 GUI 接口，可以存储图形界面的每个单元，使得 GUI 和主逻辑分离
* snake-interface.ui: GtkBuilder 构建图形框架结构
* snake.gresource.xml: GResource 文档
