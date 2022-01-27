// Travelling_salesman.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>

const int MAXN = 24; //图中点的最大数目
const int DATA_NUM = 3; //数据组数

struct graph { //图结构体
    double e[MAXN][MAXN]; //邻接矩阵
    int id[MAXN]; //图中每个点的id
    int n; //图的顶点数
    int s; //图的起点
};

graph g[DATA_NUM]; //从文件中读入的图

int city[MAXN] = {}; //city[i]为遍历的第i个点
int ans_city[MAXN] = {};
bool vis[MAXN] = {};// 某个点是否遍历过
double my_min; //搜索得到的最短路长度
int tr_tot = 0; //搜索树的节点数
double uplim = 0; //分支限界法的上界

struct state { //分支限界法用来储存状态的结构体
    int city[MAXN]; //等同于回溯法的city数组
    int now; //当前已遍历过的点的数量
    double dis; ////当前走过的距离
};
std::deque<state>q; //分支限界法用来储存状态的双端队列

void getData(int num); //从文件读入数据，参数为数据组数
void dfs(graph* a, int now, double dis);//回溯算法，参数分别为图、当前走到了第几个点,当前走的距离
void Backtracking(graph* a); //回溯算法，参数为图，在该函数中cout搜索时间
bool greedy_dfs(graph* a, int now, double dis); //贪心算法求上界
void bfs(graph* a); //分支限界法，参数为图，在该函数中cout搜索时间
void MyClear(); //初始化全局变量

void getData(int num) {
    std::fstream data;
    for (int k = 0; k < num; ++k) {
        std::string name = "graph";
        char id = '1' + k;
        name = name + id;
        name = name + ".txt";
        data.open(name);
        data >> g[k].n;
        for (int i = 0; i < g[k].n; ++i) {
            data >> g[k].id[i];
            if (g[k].id[i] == 567443)g[k].s = i;
            for (int j = 0; j < g[k].n; ++j) {
                data >> g[k].e[i][j];
                if (g[k].e[i][j] == 99999)g[k].e[i][j] = -1;
            }
        }
        data.close();
    }
}
void dfs(graph* a, int now, double dis) {
    ++tr_tot;
    if (dis > my_min)return;
    if (now == (*a).n) {
        double ans = dis + (*a).e[city[now - 1]][0];
        if ((*a).e[city[now - 1]][0] > 0 && my_min > ans) {
            my_min = ans;
            for (int i = 0; i < (*a).n; ++i)ans_city[i] = city[i];
        }
        return;
    }
    for (int i = 0; i < (*a).n; ++i) {
        if (vis[i])continue;
        if ((*a).e[city[now - 1]][i]<0)continue;
        if (dis + (*a).e[city[now - 1]][i] > my_min)continue;
        city[now] = i;
        vis[i] = 1;
        dfs(a, now + 1, dis + (*a).e[city[now - 1]][i]);
        vis[i] = 0;
    }
    return;
}
bool greedy_dfs(graph* a, int now, double dis,int x) {
    if (now == (*a).n) {
        double ans = dis + (*a).e[x][0];
        if ((*a).e[x][0] > 0) {
            uplim = dis + (*a).e[x][0];
            return 1;
        }
        return 0;
    }
    for (int i = 0; i < (*a).n; ++i) {
        if (vis[i])continue;
        if ((*a).e[x][i] < 0)continue;
        vis[i] = 1;
        bool f = greedy_dfs(a, now + 1, dis + (*a).e[x][i],i);
        vis[i] = 0;
        if (f)return 1;
    }
    return 0;
}
void bfs(graph* a) {
    while (!q.empty())q.pop_back();
    state x, y;
   
    int start_time = clock();

    vis[(*a).s] = 1;
    greedy_dfs(a, 1, 0, (*a).s);
    std::cout << "分支限界法上界为" << uplim << std::endl;
    vis[(*a).s] = 0;
   
    x.now = 1;
    x.city[0] = (*a).s;
    x.dis = 0;
    q.push_back(x);
    while (!q.empty()) {
        x = q.front();
        q.pop_front();
        //std::cout << x.now << std::endl;
        ++tr_tot;
        if (x.dis > my_min)continue;
        if (x.now == (*a).n) {
            double ans = x.dis + (*a).e[x.city[x.now - 1]][0];
            if ((*a).e[x.city[x.now - 1]][0] > 0 && my_min > ans) {
                my_min = ans;
                for (int i = 0; i < (*a).n; ++i)ans_city[i] = x.city[i];
            }
            continue;
        }
        y = x;
        y.now = x.now + 1;
        for (int i = 0; i < x.now; ++i) {
            vis[x.city[i]] = 1;
        }
        double downlim = 0;
        //std::cout << downlim << std::endl;
        for (int i = 0; i < (*a).n; ++i) {
            if (vis[i] && i != 0)continue;
            double min1 = -1;
            for (int j = i+1; j < (*a).n; ++j) {
                if (vis[j] && j != 0)continue;
                if ((*a).e[i][j] < 0)continue;
                double v = (*a).e[i][j];
                if (min1 < 0 || min1>v)min1 = (*a).e[i][j];
            }downlim += min1;
        }
        if (downlim > uplim) {
            for (int i = 0; i < x.now; ++i) {
                vis[x.city[i]] = 0;
            }
            continue;
        }
        for (int i = 0; i < (*a).n; ++i) {
            if (vis[i])continue;
            if ((*a).e[x.city[x.now - 1]][i] < 0)continue;
            y.dis = x.dis + (*a).e[x.city[x.now - 1]][i];
            if (y.dis > my_min)continue;
            y.city[x.now] = i;
            q.push_back(y);
        }
        for (int i = 0; i < x.now; ++i) {
            vis[x.city[i]] = 0;
        }
    }
    std::cout << "搜索时间为" << clock() - start_time << "ms," << "搜索树有" << tr_tot << "个节点\n";
}
void Backtracking(graph* a) {
    int start_time = clock();
    city[0] = (*a).s;
    vis[city[0]] = 1;
    dfs(a, 1, 0);
    vis[city[0]] = 0;
    std::cout << "搜索时间为" << clock() - start_time << "ms," << "搜索树有" << tr_tot << "个节点\n";
}

void MyClear() {
    for (int i = 0; i < MAXN; ++i) { city[i] = -1; vis[i] = 0; }
    tr_tot = 0;
    my_min = 999990000;
}

int main()
{
    std::cout << "开始读入数据\n\n";
    getData(DATA_NUM);
    for (int i = 0; i < DATA_NUM; ++i) {
        std::cout << "开始计算第" << i + 1 << "组数据的旅行家问题\n";
        std::cout << "图中共" << g[i].n << "个点\n";
        std::cout << "起始点为第" << g[i].s + 1 << "个点\n";

        MyClear();
        int disnow = 0;
        std::cout << "开始运行回溯法\n";
        Backtracking(&g[i]);
        std::cout << "最短路长度为:" << my_min << "\n";
        std::cout << "最短路径为:";
        for (int j = 0; j < g[i].n - 1; ++j) {
            std::cout << ans_city[j]+1 << " -> ";
        }
        std::cout << ans_city[g[i].n - 1]+1 << "\n";

        if (i < 1) {
            MyClear();
            std::cout << "开始运行分支限界法\n";
            bfs(&g[i]);
            std::cout << "最短路长度为:" << my_min << "\n";
            std::cout << "最短路径为:";
            for (int j = 0; j < g[i].n - 1; ++j) {
                std::cout << ans_city[j]+1 << " -> ";
            }
            std::cout << ans_city[g[i].n - 1]+1 << "\n";
        }
        std::cout << std::endl;
    }
    return 0;
}