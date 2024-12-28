#include <gtk/gtk.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int name_num = 0;
int seed = 0;
int T = 3;
vector<int> List;
vector<string> names;

int get_file_lines(const string &filename) {
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
    name_num = get_file_lines("./name.txt");
    if (name_num <= 0) {
        cerr << "文件读取错误或文件为空" << endl;
        return;
    }
    seed = rand() % name_num + 1;
    ifstream fin("./name.txt");
    names.resize(name_num + 1);
    for (int i = 1; i <= name_num; i++) {
        getline(fin, names[i]);
    }
    fin.close();
}

int turns() {
    unsigned int hash = 11451;
    for (char c : names[seed]) {
        hash = ((hash << 5) + hash) + c;
    }
    seed = (seed + (hash * rand()) % (name_num - 1)) % name_num + 1;
    return seed;
}

gboolean apply_animation(gpointer data) {
    GtkWidget *widget = GTK_WIDGET(data);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(context, "slide-in-active");
    return G_SOURCE_REMOVE; // 停止计时器
}
/*
gboolean show_name_label(gpointer data){
	GtkWidget *label = GTK_WIDGET(data);
	gtk_widget_(label);
	return G_SOURCE_REMOVE;
}
*/
void on_draw_button_clicked(GtkWidget *widget, gpointer data) {
    init();
    if (name_num <= 0) {
        return;
    }

    List.clear();
    List.resize(T);
    sleep(1);

    for (int i = 0; i < T; ) {
        int candidate = turns();
        bool duplicate = false;
        for (int j = 0; j < i; j++) {
            if (List[j] == candidate) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            List[i] = candidate;
            i++;
        }
    }

    GtkWidget *result_box = GTK_WIDGET(data);
    GList *children = gtk_container_get_children(GTK_CONTAINER(result_box));
    for (GList *child = children; child != NULL; child = g_list_next(child)) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }
    g_list_free(children);

    GtkWidget *label = gtk_label_new("恭喜以下人员抽中了：");
    PangoAttrList *attr_list = pango_attr_list_new();
    PangoAttribute *attr_size = pango_attr_size_new(24 * PANGO_SCALE);
    pango_attr_list_insert(attr_list, attr_size);
    gtk_label_set_attributes(GTK_LABEL(label), attr_list);
    gtk_box_pack_start(GTK_BOX(result_box), label, TRUE, TRUE, 0);

    for (int i = 0; i < T; i++) {
        string result_text = names[List[i]];
        GtkWidget *name_label = gtk_label_new(result_text.c_str());

        // 设置Pango文字样式
        PangoAttrList *attr_list_name = pango_attr_list_new();
        PangoAttribute *attr_size_name = pango_attr_size_new(36 * PANGO_SCALE);
        pango_attr_list_insert(attr_list_name, attr_size_name);
        gtk_label_set_attributes(GTK_LABEL(name_label), attr_list_name);

        // 设置CSS样式
        GtkStyleContext *context = gtk_widget_get_style_context(name_label);
        gtk_style_context_add_class(context, "slide-in");
        gtk_box_pack_start(GTK_BOX(result_box), name_label, TRUE, TRUE, 0);

        // 延迟添加"slide-in-active"类以实现动画效果
        g_timeout_add(50 * (i + 1), apply_animation, name_label);
        
        sleep(0.5);
    }

    GtkWidget *again_button = gtk_button_new_with_label("再来一次");
    gtk_widget_set_size_request(again_button, 100, 50);
    g_signal_connect(again_button, "clicked", G_CALLBACK(on_draw_button_clicked), result_box);
    gtk_box_pack_start(GTK_BOX(result_box), again_button, TRUE, FALSE, 0);

    GtkWidget *reset_button = gtk_button_new_with_label("退出");
    gtk_widget_set_size_request(reset_button, 100, 50);
    g_signal_connect(reset_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_start(GTK_BOX(result_box), reset_button, TRUE, FALSE, 0);

    gtk_widget_show_all(GTK_WIDGET(result_box));
}

void on_submit_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *spin_button = GTK_WIDGET(data);
    T = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_button));
    if (T < 1) {
        T = 1;
    }
    if (T > 6) {
        T = 6;
    }
    on_draw_button_clicked(widget, data);
}

int main(int argc, char *argv[]) {
    srand(time(0)); // 初始化随机数种子

    gtk_init(&argc, &argv);

    GtkCssProvider *provider = gtk_css_provider_new();
    GFile *css_file = g_file_new_for_path("style.css");
    gtk_css_provider_load_from_file(provider, css_file, NULL);
    g_object_unref(css_file);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "抽奖程序");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    GtkWidget *vbox_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox_left, FALSE, TRUE, 0);

    GtkWidget *label = gtk_label_new("请选择一次抽多少个人：");
    gtk_box_pack_start(GTK_BOX(vbox_left), label, TRUE, TRUE, 0);
    GtkWidget *label_2 = gtk_label_new("（已重复的人已删去）");
    gtk_box_pack_start(GTK_BOX(vbox_left), label_2, TRUE, TRUE, 0);
    GtkWidget *spin_button = gtk_spin_button_new_with_range(1, 6, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_button), 3);
    gtk_box_pack_start(GTK_BOX(vbox_left), spin_button, FALSE, FALSE, 0);

    GtkWidget *submit_button = gtk_button_new_with_label("确定");
    gtk_widget_set_size_request(submit_button, 100, 50);
    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_submit_button_clicked), spin_button);
    gtk_box_pack_start(GTK_BOX(vbox_left), submit_button, FALSE, FALSE, 0);

    GtkWidget *vbox_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox_right, TRUE, TRUE, 0);

    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_draw_button_clicked), vbox_right);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
