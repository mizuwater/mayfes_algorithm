#include<iostream>
#include<algorithm>
#include<vector>
#include<queue>
#include<cmath>
#include<functional>
#include <random>
//枠の長さ
const int N = 100;
//でかい数
const int M = 10000;
using namespace std;

//頂点の構造体
struct node{
    int key;
    bool dot;
    bool left, right, up, down;
};

node a[N][N]; //入力グラフの配列
bool b[N][N]; //幅優先探索で使う、絵画に含まれる頂点なら最初1で、連結成分検出の際に訪れたら0にする
node c[2*N][2*N]; //最終的なグラフの配列

int xmin, xmax, ymin, ymax; //迷路の右端、左端、下端、上端
pair<int, int> start, goal; //配列cの迷路におけるスタート、ゴールの座標
pair<int, int> searchv; //deadendmoveで次数3以上の頂点を見つけるために使う頂点座標
int direction; //deadendmoveで前にどの方向から来たかを記録する、0は下、1は上、2は左、3は右
queue<pair<int, int> > rightway; //解経路

//unionfind一式
int par[4*N*N];
void init(int n){
    for(int i = 0; i < n; i++)par[i] = i;
}
int root(int x){
    if(par[x] == x){
        return x;
    }
    else{
        return par[x] = root(par[x]);
    }
}
bool same(int x, int y){
    return root(x) == root(y);
}
void unite(int x, int y){
    x = root(x);
    y = root(y);
    if(x==y)return;
    par[x] = y;
}

//与えられた頂点から幅優先探索して連結成分全体を探索する関数
int bfs(int x, int y){
    b[x][y] = 0; //(x,y)は訪問済みとする
    queue<pair<int, int> > que;
    int ccsize = 0; //連結成分のサイズ
    que.push(make_pair(x,y));
    pair<int, int> vertex;
    //queが空になるまで幅優先探索
    while(que.size()!=0){
        vertex = que.front();
        que.pop();
        ccsize++;
        //vertexの左にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].left==1 && b[vertex.first-1][vertex.second]==1){
            b[vertex.first-1][vertex.second] = 0;
            que.push(make_pair(vertex.first-1, vertex.second));
        }
        //vertexの右にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].right==1 && b[vertex.first+1][vertex.second]==1){
            b[vertex.first+1][vertex.second] = 0;
            que.push(make_pair(vertex.first+1, vertex.second));
        }
        //vertexの上にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].up==1 && b[vertex.first][vertex.second+1]==1){
            b[vertex.first][vertex.second+1] = 0;
            que.push(make_pair(vertex.first, vertex.second+1));
        }
        //vertexの下にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].down==1 && b[vertex.first][vertex.second-1]==1){
            b[vertex.first][vertex.second-1] = 0;
            que.push(make_pair(vertex.first, vertex.second-1));
        }
    }
    return ccsize; //連結成分のサイズを返す
}

//与えられた頂点から幅優先探索して連結成分全体を探索し、それをグラフから削除する関数
void bfsdelete(int x, int y){
    queue<pair<int, int> > que;
    que.push(make_pair(x,y));
    pair<int, int> vertex;
    //queが空になるまで幅優先探索
    while(que.size()!=0){
        vertex = que.front();
        que.pop();
        a[vertex.first][vertex.second].dot = 0;
        //vertexの左にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].left==1){
            a[vertex.first][vertex.second].left = 0;
            if(a[vertex.first-1][vertex.second].dot==1){
                que.push(make_pair(vertex.first-1, vertex.second));
            }
        }
        //vertexの右にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].right==1){
            a[vertex.first][vertex.second].right = 0;
            if(a[vertex.first+1][vertex.second].dot==1){
                que.push(make_pair(vertex.first+1, vertex.second));
            }
        }
        //vertexの上にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].up==1){
            a[vertex.first][vertex.second].up = 0;
            if(a[vertex.first][vertex.second+1].dot==1){
                que.push(make_pair(vertex.first, vertex.second+1));
            }
        }
        //vertexの下にまだ探索していない、辺のつながった頂点があるなら
        if(a[vertex.first][vertex.second].down==1){
            a[vertex.first][vertex.second].down = 0;
            if(a[vertex.first][vertex.second-1].dot==1){
                que.push(make_pair(vertex.first, vertex.second-1));
            }
        }
    }
}

//最大連結成分以外の頂点を削除して、最大連結成分の頂点数を返す関数
int maxcc(){
    vector<pair<int, pair<int, int> > > v;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(b[i][j]==1){
                v.push_back(make_pair(bfs(i,j), make_pair(i,j)));
            }
        }
    }
    //ソートして最大以外無視
    sort(v.begin(), v.end());
    int maxccsize = v[v.size()-1].first;
    v.pop_back();
    pair<int, int> vertex;
    //最大以外無視
    while(v.size()!=0){
        vertex = v[v.size()-1].second;
        v.pop_back();
        bfsdelete(vertex.first, vertex.second);
    }
    return maxccsize;
}

//ランダムな全域木を作る関数
void rst(){
    vector<pair<pair<int, int>, pair<int, int> > > edge;
    int vnumber = 0;
    //辺をvectorで管理する
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(a[i][j].dot==1){
                //頂点に番号を振る
                a[i][j].key = vnumber;
                vnumber++;
                if(a[i][j].right==1){
                    edge.push_back(make_pair(make_pair(i,j), make_pair(i+1,j)));
                }
                if(a[i][j].up==1){
                    edge.push_back(make_pair(make_pair(i,j), make_pair(i,j+1)));
                }
            }
        }
    }
    //辺をランダムに並べる
    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(edge.begin(), edge.end(), engine);
    //全域木を構成
    init(N*N);
    pair<int, int> v1;
    pair<int, int> v2;
    while(edge.size()!=0){
        v1 = edge[edge.size()-1].first;
        v2 = edge[edge.size()-1].second;
        edge.pop_back();
        //v1とv2がすでに連結ならv1とv2を結ぶ辺を削除する
        if(same(a[v1.first][v1.second].key, a[v2.first][v2.second].key)){
            //v1の上にv2がある場合
            if(v1.first==v2.first){
                a[v1.first][v1.second].up = 0;
                a[v2.first][v2.second].down = 0;
            }
            //v1の右にv2がある場合
            if(v1.second==v2.second){
                a[v1.first][v1.second].right = 0;
                a[v2.first][v2.second].left = 0;
            }
        }
        //v1とv2がまだ連結でないならv1とv2を結ぶ辺を加えるのでv1とv2をuniteする
        else{
            unite(a[v1.first][v1.second].key, a[v2.first][v2.second].key);
        }
    }
}

//与えられた座標がスタート、ゴールの迷路の解経路をつくる関数、cが解経路を表すグラフで、aがもとの全域木
void makeway(int x, int y){
    pair<int, int> vertex; //現在地（ハミルトン路をまわっていく）
    vertex = make_pair(2*x, 2*y+1);
    //上下左右につながっている頂点がなくなるまでめぐる
    while(1){
        //訪問した頂点を記録
        c[vertex.first][vertex.second].dot = 1;
        rightway.push(vertex);
        //ゴールの座標(2*x+1, 2*y)についたら終了
        if(vertex==make_pair(2*x+1, 2*y+1)) break;
        //4等分したマスのうち左上にいる場合、全域木で辺がつながっているなら左のマスに移動し、つながっていないなら下のマスに移動する
        if(vertex.first%2==0 && vertex.second%2==1){
            if(a[vertex.first/2][vertex.second/2].left==1){
                c[vertex.first][vertex.second].left = 1;
                c[vertex.first-1][vertex.second].right = 1;
                vertex = make_pair(vertex.first-1, vertex.second);
                continue;
            }
            else{
                c[vertex.first][vertex.second].down = 1;
                c[vertex.first][vertex.second-1].up = 1;
                vertex = make_pair(vertex.first, vertex.second-1);
                continue;
            }
        }
        //4等分したマスのうち右上にいる場合、全域木で辺がつながっているなら上のマスに移動し、つながっていないなら左のマスに移動する
        if(vertex.first%2==1 && vertex.second%2==1){
            if(a[vertex.first/2][vertex.second/2].up==1){
                c[vertex.first][vertex.second].up = 1;
                c[vertex.first][vertex.second+1].down = 1;
                vertex = make_pair(vertex.first, vertex.second+1);
                continue;
            }
            else{
                c[vertex.first][vertex.second].left = 1;
                c[vertex.first-1][vertex.second].right = 1;
                vertex = make_pair(vertex.first-1, vertex.second);
                continue;
            }
        }
        //4等分したマスのうち左下にいる場合、全域木で辺がつながっているなら下のマスに移動し、つながっていないなら右のマスに移動する
        if(vertex.first%2==0 && vertex.second%2==0){
            if(a[vertex.first/2][vertex.second/2].down==1){
                c[vertex.first][vertex.second].down = 1;
                c[vertex.first][vertex.second-1].up = 1;
                vertex = make_pair(vertex.first, vertex.second-1);
                continue;
            }
            else{
                c[vertex.first][vertex.second].right = 1;
                c[vertex.first+1][vertex.second].left = 1;
                vertex = make_pair(vertex.first+1, vertex.second);
                continue;
            }
        }
        //4等分したマスのうち右下にいる場合、全域木で辺がつながっているなら右のマスに移動し、つながっていないなら上のマスに移動する
        if(vertex.first%2==1 && vertex.second%2==0){
            if(a[vertex.first/2][vertex.second/2].right==1){
                c[vertex.first][vertex.second].right = 1;
                c[vertex.first+1][vertex.second].left = 1;
                vertex = make_pair(vertex.first+1, vertex.second);
                continue;
            }
            else{
                c[vertex.first][vertex.second].up = 1;
                c[vertex.first][vertex.second+1].down = 1;
                vertex = make_pair(vertex.first, vertex.second+1);
                continue;
            }
        }
    }
}

//迷路の解経路をつくる関数
void makemazeway(){
    for(int j = N-1; j >= 0; j--){
        for(int i = 0; i < N; i++){
            if(a[i][j].dot==1){
                //解経路のスタート、ゴール地点を(i,j)にする
                start = make_pair(2*i, 2*j+1);
                goal = make_pair(2*i+1, 2*j+1);
                makeway(i,j);
                i = N;
                j = -1;
            }
        }
    }
}

//迷路の地の部分をつくる関数
void makemazeblank(){
    //まずは迷路のサイズを定める
    //左端の座標を決める
    for(int i = 0; i < 2*N; i++){
        for(int j = 0; j < 2*N; j++){
            if(c[i][j].dot==1){
                xmin = i;
                i = 2*N;
                j = 2*N;
            }
        }
    }
    //右端の座標を決める
    for(int i = 2*N-1; i >= 0; i--){
        for(int j = 0; j < 2*N; j++){
            if(c[i][j].dot==1){
                xmax = i;
                i = -1;
                j = 2*N;
            }
        }
    }
    //下端の座標を決める
    for(int j = 0; j < 2*N; j++){
        for(int i = 0; i < 2*N; i++){
            if(c[i][j].dot==1){
                ymin = j;
                i = 2*N;
                j = 2*N;
            }
        }
    }
    //上端の座標を決める
    for(int j = 2*N-1; j>=0; j--){
        for(int i = 0; i < 2*N; i++){
            if(c[i][j].dot==1){
                ymax = j;
                i = 2*N;
                j = -1;
            }
        }
    }
    //辺をベクターで管理                
    vector<pair<pair<int, int>, pair<int, int> > > edge;
    int vnumber = 1;
    for(int i = xmin; i <= xmax; i++){
        for(int j = ymin; j <= ymax; j++){
            if(c[i][j].dot==1){
                c[i][j].key = 0; //迷路の解経路の部分は連結
                if(i<xmax){
                    edge.push_back(make_pair(make_pair(i,j), make_pair(i+1,j)));
                }
                if(j<ymax){
                    edge.push_back(make_pair(make_pair(i,j), make_pair(i,j+1)));
                }
                continue;
            }
            if(c[i][j].dot!=1){
                c[i][j].dot = 1; //地の部分も迷路に入れる
                //頂点に番号を振る
                c[i][j].key = vnumber;
                vnumber++;
                if(i<xmax){
                    edge.push_back(make_pair(make_pair(i,j), make_pair(i+1,j)));
                }
                if(j<ymax){
                    edge.push_back(make_pair(make_pair(i,j), make_pair(i,j+1)));
                }
            }
        }
    }
    //辺をランダムに並べる
    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(edge.begin(), edge.end(), engine);
    //地の部分の迷路を構成
    init(4*N*N);
    pair<int, int> v1;
    pair<int, int> v2;
    random_device rnd; //乱数
    while(edge.size()!=0){
        v1 = edge[edge.size()-1].first;
        v2 = edge[edge.size()-1].second;
        edge.pop_back();
        //v1とv2がまだ連結でないなら、v1とv2を結ぶ辺を迷路に加え、v1とv2をuniteする
        if(!same(c[v1.first][v1.second].key, c[v2.first][v2.second].key)){
            //v1の上にv2がある場合
            if(v1.first==v2.first){
                c[v1.first][v1.second].up = 1;
                c[v2.first][v2.second].down = 1;
                unite(c[v1.first][v1.second].key, c[v2.first][v2.second].key);
                while(v2.second < ymax && !same(c[v2.first][v2.second].key, c[v2.first][v2.second+1].key)){
                    c[v2.first][v2.second].up = 1;
                    c[v2.first][v2.second+1].down = 1;
                    unite(c[v2.first][v2.second].key, c[v2.first][v2.second+1].key);
                    v2.second = v2.second+1;
                    if(rnd()%((xmax-xmin+ymax-ymin)/8)==0) break;
                }
            }
            //v1の右にv2がある場合
            if(v1.second==v2.second){
                c[v1.first][v1.second].right = 1;
                c[v2.first][v2.second].left = 1;
                unite(c[v1.first][v1.second].key, c[v2.first][v2.second].key);
                while(v2.first < xmax && !same(c[v2.first][v2.second].key, c[v2.first+1][v2.second].key)){
                    c[v2.first][v2.second].right = 1;
                    c[v2.first+1][v2.second].left = 1;
                    unite(c[v2.first][v2.second].key, c[v2.first+1][v2.second].key);
                    v2.first = v2.first+1;
                    if(rnd()%((xmax-xmin+ymax-ymin)/8)==0) break;
                }
            }
        }
    }
}

//次数1の2頂点を受け取って、その2頂点を結ぶ辺をつくり、代わりに新たな次数１の頂点をつくる関数
void deadendmove(int x, int y, int z, int w){
    //与えられたのがスタート、ゴールなら、deadendmoveは適用しない
    if(x==start.first && y==start.second && z==goal.first && w==goal.second){
        x = 0;
        y = 0;
        z = 2;
        w = 2;
    }
    if(x==goal.first && y==goal.second && z==start.first && w==start.second){
        x = 0;
        y = 0;
        z = 2;
        w = 2;
    }
    if(x==z && y==w-1){
        //(x,y)が(z,w)の下にあるなら、辺で結び、次数を1増やし、(z,w)からスタートして次数3以上の頂点を探す
        c[x][y].up = 1;
        c[z][w].down = 1;
        c[x][y].key++;
        c[z][w].key++;
        searchv = make_pair(z,w);
        direction = 0;
        while(1){
            //もし下からきたなら
            if(direction==0){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし上からきたなら
            if(direction==1){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
            }
            //もし左からきたなら
            if(direction==2){
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし右からきたなら
            if(direction==3){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
        }
    }
    if(x==z && y==w+1){
        //(x,y)が(z,w)の上にあるなら、辺で結び、次数を1増やし、(z,w)からスタートして次数3以上の頂点を探す
        c[x][y].down = 1;
        c[z][w].up = 1;
        c[x][y].key++;
        c[z][w].key++;
        searchv = make_pair(z,w);
        direction = 1;
        while(1){
            //もし下からきたなら
            if(direction==0){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし上からきたなら
            if(direction==1){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
            }
            //もし左からきたなら
            if(direction==2){
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし右からきたなら
            if(direction==3){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
        }
    }
    if(x==z-1 && y==w){
        //(x,y)が(z,w)の左にあるなら、辺で結び、次数を1増やし、(z,w)からスタートして次数3以上の頂点を探す
        c[x][y].right = 1;
        c[z][w].left = 1;
        c[x][y].key++;
        c[z][w].key++;
        searchv = make_pair(z,w);
        direction = 2;
        while(1){
            //もし下からきたなら
            if(direction==0){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし上からきたなら
            if(direction==1){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
            }
            //もし左からきたなら
            if(direction==2){
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし右からきたなら
            if(direction==3){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
        }
    }
    if(x==z+1 && y==w){
        //(x,y)が(z,w)の右にあるなら、辺で結び、次数を1増やし、(z,w)からスタートして次数3以上の頂点を探す
        c[x][y].left = 1;
        c[z][w].right = 1;
        c[x][y].key++;
        c[z][w].key++;
        searchv = make_pair(z,w);
        direction = 3;
        while(1){
            //もし下からきたなら
            if(direction==0){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし上からきたなら
            if(direction==1){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
            }
            //もし左からきたなら
            if(direction==2){
                //もし右の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].right==1){
                    //もし右の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first+1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].right = 0;
                        c[searchv.first+1][searchv.second].left = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first+1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //右の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first+1, searchv.second);
                        direction = 2;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
            //もし右からきたなら
            if(direction==3){
                //もし左の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].left==1){
                    //もし左の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first-1][searchv.second].key>=3){
                        c[searchv.first][searchv.second].left = 0;
                        c[searchv.first-1][searchv.second].right = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first-1][searchv.second].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //左の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first-1, searchv.second);
                        direction = 3;
                        continue;
                    }
                }
                //もし下の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].down==1){
                    //もし下の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second-1].key>=3){
                        c[searchv.first][searchv.second].down = 0;
                        c[searchv.first][searchv.second-1].up = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second-1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //下の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second-1);
                        direction = 1;
                        continue;
                    }
                }
                //もし上の頂点との間に辺があるなら
                if(c[searchv.first][searchv.second].up==1){
                    //もし上の頂点の次数が3以上なら、その頂点との間の辺を削除し、次数を更新し、次数1になったsearchvのまわりに次数1の頂点がないか確認する
                    if(c[searchv.first][searchv.second+1].key>=3){
                        c[searchv.first][searchv.second].up = 0;
                        c[searchv.first][searchv.second+1].down = 0;
                        c[searchv.first][searchv.second].key--;
                        c[searchv.first][searchv.second+1].key--;
                        //次数1の頂点が右にあれば
                        if(searchv.first+1!=2*N && c[searchv.first+1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first+1, searchv.second);
                            break;
                        }
                        //次数1の頂点が上にあれば
                        if(searchv.second+1!=2*N && c[searchv.first][searchv.second+1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second+1);
                            break;
                        }
                        //次数1の頂点が左にあれば
                        if(searchv.first-1!=-1 && c[searchv.first-1][searchv.second].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first-1, searchv.second);
                            break;
                        }
                        //次数1の頂点が下にあれば
                        if(searchv.second-1!=-1 && c[searchv.first][searchv.second-1].key==1){
                            deadendmove(searchv.first, searchv.second, searchv.first, searchv.second-1);
                            break;
                        }
                        break;
                    }
                    //上の頂点の次数が2なら
                    else{
                        searchv = make_pair(searchv.first, searchv.second+1);
                        direction = 0;
                        continue;
                    }
                }
            }
        }
    }
}

//迷路を作り変えて絵をわかりにくくする関数
void makemazecomplex(){
    //次数をkeyに記録していく
    for(int i = xmin; i <= xmax; i++){
        for(int j = ymin; j <= ymax; j++){
            c[i][j].key = c[i][j].left + c[i][j].right + c[i][j].up + c[i][j].down;
        }
    }
    //次数が1の隣り合った頂点を検出していく
    for(int i = xmin; i <= xmax; i++){
        for(int j = ymin; j <= ymax; j++){
            if(c[i][j].key==1){
                if(c[i+1][j].key==1){
                    deadendmove(i,j,i+1,j);
                    continue;
                }
                if(c[i][j+1].key==1){
                    deadendmove(i,j,i,j+1);
                    continue;
                }
            }
        }
    }
}

int main(){
    int n, m, x, y, z, w;
    cin >> n;
    //初期化する
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            a[i][j].dot = 0;
            a[i][j].left = 0;
            a[i][j].right = 0;
            a[i][j].up = 0;
            a[i][j].down = 0;
            b[i][j] = 0;
        }
    }
    for(int i = 0; i < 2*N; i++){
        for(int j = 0; j < 2*N; j++){
            c[i][j].dot = 0;
            c[i][j].left = 0;
            c[i][j].right = 0;
            c[i][j].up = 0;
            c[i][j].down = 0;
        }
    } 
    //頂点の入力
    for(int i = 0; i < n; i++){
        cin >> x >> y;
        a[x][y].dot = 1;
        b[x][y] = 1;
    }
    cin >> m; 
    //辺の入力
    for(int i = 0; i < m; i++){
        cin >> x >> y >> z >> w;
        if(x==z){
            if(y==w+1){
                a[x][y].down = 1;
                a[z][w].up = 1;
            }
            if(y==w-1){
                a[x][y].up = 1;
                a[z][w].down = 1;
            }
        }
        if(y==w){
            if(x==z+1){
                a[x][y].left = 1;
                a[z][w].right = 1;
            }
            if(x==z-1){
                a[x][y].right = 1;
                a[z][w].left = 1;
            }
        }
    }
    //最大連結成分以外は削除、削除後のグラフの頂点数を求める
    int graphsize = maxcc();
    //ランダムな全域木をつくる
    rst();
    //全域木から迷路の解経路をつくる
    makemazeway();
    //迷路の地の部分をつくる
    makemazeblank();
    //配列cのkeyを次数にするため、使わない頂点のkeyをMにすることで区別する
    for(int i = 0; i < 2*N; i++){
        for(int j = 0; j < 2*N; j++){
            c[i][j].key = M;
        }
    }
    //ヒューリスティックに迷路を作り変えて絵をわかりにくくする
    makemazecomplex();
    //迷路の表示、#と空白によるもの
    for(int j = ymax*2+2; j >= ymin*2; j--){
        for(int i = xmin*2; i <= xmax*2+2; i++){
            if(j==ymax*2+2 && i==start.first*2+1){
                cout << " ";
                continue;
            }
            if(j==ymax*2+2 && i==goal.first*2+1){
                cout << " ";
                continue;
            }
            if(i==xmin*2 || i==xmax*2+2 || j==ymax*2+2 || j==ymin*2){
                cout << "#";
                continue;
            }
            if(i%2==1 && j%2==1){
                if(c[i/2][j/2].dot==1){
                    cout << " ";
                    continue;
                }
                else{
                    cout << "#";
                    continue;
                }
            }
            if(i%2==0 && j%2==1){
                if(c[(i-1)/2][j/2].right==1){
                    cout << " ";
                    continue;
                }
                else{
                    cout << "#";
                    continue;
                }
            }
            if(i%2==1 && j%2==0){
                if(c[i/2][(j+1)/2].down==1){
                    cout << " ";
                    continue;
                }
                else{
                    cout << "#";
                    continue;
                }
            }
            if(i%2==0 && j%2==0) cout << "#";
        }
        cout << endl;
    }

    //縦横の頂点数、辺（頂点の組で表す）の表示
    cout << xmax-xmin+1 << endl; //横の頂点数
    cout << ymax-ymin+1 << endl; //縦の頂点数
    // ここから辺の表示　例えば頂点(1,2)と頂点(2,2)の間に辺があるなら　1 2 2 2　のように出力する。辺ごとに改行する
    // for(int i = xmin; i <= xmax; i++){
    //     for(int j = ymin; j <= ymax; j++){
    //         if(c[i][j].right==1){
    //             cout << i-xmin  << " " << j-ymin << " " << i-xmin+1 << " " << j-ymin << endl;
    //         }
    //         if(c[i][j].up==1){
    //             cout << i-xmin  << " " << j-ymin << " " << i-xmin << " " << j-ymin+1 << endl;
    //         }
    //     }
    // }

    //解経路も出力してみる　頂点の座標を、解経路をスタートからたどる順に出力していく
    // pair<int, int> rightwayvertex;
    // while(!rightway.empty()){
    //     rightwayvertex = rightway.front();
    //     cout << rightwayvertex.first-xmin << " " << rightwayvertex.second-ymin << endl;
    //     rightway.pop();
    // }
    return 0;
}