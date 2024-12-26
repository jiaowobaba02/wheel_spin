#include<bits/stdc++.h>
#include<gtk/gtk.h>
#define int long long
using namespace std;

int name_num = 0;
int seed = 0;
int T = 3;
string names[114514];

int get_file_lines(const string &filename) {
    //识别名字文件有多少行
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开：" << filename << " 请检查是否有其他资源占用或权限是否不足" << endl;
        return -1;
    }
    int line_num = 0;
    string line;
    while (getline(file, line)) {
        line_num++;
    }
    file.close();
    return line_num;
}

void init() {
    // 初始化
    srand(time(0));
    name_num = get_file_lines("./name.txt");
    seed = rand() % name_num + 1;
    ifstream fin("./name.txt");
    for (int i = 1; i <= name_num; i++) {
        fin >> names[i];
    }
}

int turns() {
    srand(time(0));
    long long hash = 11451;
    for (char c : names[seed]) {
        hash = ((hash << 5) + hash) + c; //对名字取哈希值，增加随机性
    }
    seed = (seed + (hash * rand()) % (name_num - 1)) % name_num + 1; //防止上次所选的人与这次一样
    return seed;
}

// 前置声明
void on_confirm_clicked(GtkWidget *widget, gpointer data);
void on_reset_clicked(GtkWidget *widget, gpointer data);

// 前端实现
GtkWidget *window;
GtkWidget *main_box;
GtkWidget *label;
GtkWidget *spin_button;
GtkWidget *button_confirm;
GtkWidget *button_retry;
GtkWidget *button_reset;

void show_selected_name(int selected) {
    // 创建一个水平排列的 box
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), hbox, FALSE, FALSE, 0);

    // 显示滚动条上方的名字（浅色）
    stringstream ss_up;
    ss_up << names[(selected - 2 + name_num) % name_num + 1];
    string result_up = ss_up.str();

    GtkWidget *label_up = gtk_label_new(result_up.c_str());
    PangoAttrList *attr_list_up = pango_attr_list_new();
    PangoAttribute *attr_alpha_up = pango_attr_foreground_alpha_new(32768);  // 浅色透明度
    pango_attr_list_insert(attr_list_up, attr_alpha_up);
    gtk_label_set_attributes(GTK_LABEL(label_up), attr_list_up);
    gtk_box_pack_start(GTK_BOX(hbox), label_up, FALSE, FALSE, 0);
    gtk_widget_show(label_up);
    pango_attr_list_unref(attr_list_up);  // 释放属性列表

    // 显示滚动条中间的名字（深色）
    stringstream ss_mid;
    ss_mid << names[selected];
    string result_mid = ss_mid.str();

    GtkWidget *label_mid = gtk_label_new(result_mid.c_str());
    PangoAttrList *attr_list_mid = pango_attr_list_new();
    PangoAttribute *attr_alpha_mid = pango_attr_foreground_alpha_new(65535);  // 深色透明度
    pango_attr_list_insert(attr_list_mid, attr_alpha_mid);
    gtk_label_set_attributes(GTK_LABEL(label_mid), attr_list_mid);
    gtk_box_pack_start(GTK_BOX(hbox), label_mid, FALSE, FALSE, 0);
    gtk_widget_show(label_mid);
    pango_attr_list_unref(attr_list_mid);  // 释放属性列表

    // 显示滚动条下方的名字（浅色）
    stringstream ss_down;
    ss_down << names[(selected + name_num) % name_num + 1];
    string result_down = ss_down.str();

    GtkWidget *label_down = gtk_label_new(result_down.c_str());
    PangoAttrList *attr_list_down = pango_attr_list_new();
    PangoAttribute *attr_alpha_down = pango_attr_foreground_alpha_new(32768);  // 浅色透明度
    pango_attr_list_insert(attr_list_down, attr_alpha_down);
    gtk_label_set_attributes(GTK_LABEL(label_down), attr_list_down);
    gtk_box_pack_start(GTK_BOX(hbox), label_down, FALSE, FALSE, 0);
    gtk_widget_show(label_down);
    pango_attr_list_unref(attr_list_down);  // 释放属性列表
}

void show_second_screen() {
    // 显示第二屏
    gtk_container_foreach(GTK_CONTAINER(main_box), (GtkCallback)gtk_widget_hide, NULL);

    GtkWidget *second_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), second_box, TRUE, TRUE, 0);

    label = gtk_label_new("恭喜以下人员抽中了\n");
    gtk_box_pack_start(GTK_BOX(second_box), label, FALSE, FALSE, 0);
    gtk_widget_show(label);

    // 显示抽奖结果
    for (int i = 0; i < T; i++) {
        int selected = turns();
        show_selected_name(selected);

        // 添加适当的延迟，防止速度过快造成段错误
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }
        g_usleep(200000);  // 200ms 延迟
    }

    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(second_box), button_box, FALSE, FALSE, 0);

    button_retry = gtk_button_new_with_label("再来一次");
    g_signal_connect(button_retry, "clicked", G_CALLBACK(on_confirm_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button_retry, TRUE, TRUE, 0);
    gtk_widget_show(button_retry);

    button_reset = gtk_button_new_with_label("重新设置");
    g_signal_connect(button_reset, "clicked", G_CALLBACK(on_reset_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button_reset, TRUE, TRUE, 0);
    gtk_widget_show(button_reset);
}

void on_confirm_clicked(GtkWidget *widget, gpointer data) {
    T = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_button));
    if (T <= 0 || T > name_num) {
        cerr << "无效的抽奖人数，请重新选择。\n";
        return;
    }
    show_second_screen();
}

void show_first_screen() {
    gtk_container_foreach(GTK_CONTAINER(main_box), (GtkCallback)gtk_widget_hide, NULL);

    GtkWidget *first_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), first_box, TRUE, TRUE, 0);

    label = gtk_label_new("请选择一次抽多少个人\n");
    gtk_box_pack_start(GTK_BOX(first_box), label, FALSE, FALSE, 0);
    gtk_widget_show(label);

    spin_button = gtk_spin_button_new_with_range(1, 6, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), 3);
    gtk_box_pack_start(GTK_BOX(first_box), spin_button, FALSE, FALSE, 0);
    gtk_widget_show(spin_button);

    button_confirm = gtk_button_new_with_label("确定");
    g_signal_connect(button_confirm, "clicked", G_CALLBACK(on_confirm_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(first_box), button_confirm, FALSE, FALSE, 0);
    gtk_widget_show(button_confirm);
}

void on_reset_clicked(GtkWidget *widget, gpointer data) {
    show_first_screen();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    init();
    srand(time(0));
    // 创建窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "抽奖程序");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // 主容器
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    show_first_screen();

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
