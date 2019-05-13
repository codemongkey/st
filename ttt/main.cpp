#include "widget.h"
#include <QApplication>
#define bro(x) (((x)->ftr->lc == (x))?((x)->ftr->rc):((x)->ftr->lc))
typedef bool RB_COLOR;
#define RB_COLOR_RED true
#define RB_COLOR_BLACK false

template <typename T>
class redblacktree{
protected:
    struct RB_Node;
    RB_Node* _root;
    RB_Node* _hot;

    int _size;
    void init(T);
    RB_Node* zig(RB_Node*);
    RB_Node* zag(RB_Node*);

    void SolveDoubleRed(RB_Node*);
    void SolveLostBlack(RB_Node*);
    RB_Node* find(T);
    void removetree(RB_Node*);
public:
    struct iterator;
    redblacktree():_root(NULL),_hot(NULL),_size(0)
    {

    }

    iterator insert(T);
    bool remove(T);
    bool remove(iterator&);
    iterator search(T);
    iterator lower_bound(T);
    iterator upper_bound(T);

    void clear();//dfs

    int size();
    bool empty();
    iterator begin();
    static iterator end();

};

template <typename T>
struct redblacktree<T>::RB_Node{
  T val;
  RB_COLOR RBc;
  RB_Node* ftr;
  RB_Node* lc;
  RB_Node* rc;

  RB_Node(T v= T(),RB_COLOR RB = RB_COLOR_RED,RB_Node*  f=nullptr,
          RB_Node* lc = nullptr,RB_Node* rc = nullptr)
         :val(v),RBc(RB),ftr(f),lc(lc),rc(rc)
  {}

  RB_Node* succ(){
      RB_Node* ptn = rc;
      if(!ptn)
          return nullptr;
      while(ptn->lc) {
          ptn=ptn->lc;
      }
      return ptn;
  }

  RB_Node* left_Node(){
      RB_Node* ptn = this;
      if(!lc) {
          while(ptn->ftr && ptn->ftr->lc==ptn){
              ptn = ptn->ftr;
          }
          ptn = ptn->ftr;//如果是
      } else {
          ptn = ptn->lc;
          while(ptn->rc){
              ptn = ptn->rc;
          }
      }
      return ptn;
  }
  RB_Node* right_Node(){
      RB_Node* ptn = this;
      if(!rc) {
          while(ptn->ftr && ptn->ftr->rc==ptn){
              ptn = ptn->ftr;
          }
          ptn = ptn->ftr;
      } else {
          ptn = ptn->rc;
          while(ptn->lc){
              ptn = ptn->lc;
          }
      }
      return ptn;
  }
};

template <typename T>
struct redblacktree<T>::iterator{
protected:
    RB_Node* _real__node;
public:
    T operator*(){
        return _real__node->val;
    }

    bool operator==(iterator const&itr){
        return _real__node==itr._real__node;
    }

    bool operator!=(iterator const&itr){
        return _real__node!=itr._real__node;
    }

    bool operator!(){
        return !_real__node;
    }
    iterator& operator=(iterator const&itr){
        _real__node=itr._real__node;
        return *this;
    }

    iterator& operator++(){
        _real__node = _real__node->right_Node();
        return *this;
    }
    iterator& operator--(){
        _real__node = _real__node->left_Node();
        return *this;
    }
    iterator(RB_Node * node_nn=nullptr):_real__node(node_nn){}
    //iterator(T const& val_vv):_real__node(find(val_vv)){}
    iterator(iterator const& itr):_real__node(itr._real__node){}
};

template <typename T>
    typename
redblacktree<T>::RB_Node* redblacktree<T>::find(T v) {
    RB_Node* ptn = _root;
    _hot = nullptr;
    while(ptn&&ptn->val!=v) {
        _hot =ptn;
        if(ptn->val >v) {
            ptn = ptn ->lc;
        } else {
            ptn = ptn ->rc;
        }
    }
    return ptn;
}

template <typename T>
    typename
redblacktree<T>::iterator redblacktree<T>::search(T v){
    return(iterator(find(v)));
}

template <typename T>
    typename
redblacktree<T>::iterator redblacktree<T>::upper_bound(T v){
    RB_Node* ptn = _root;
    _hot = nullptr;
    while(ptn) {
        _hot =ptn;
        if(ptn->val >v) {
            ptn = ptn ->lc;
        } else {
            ptn = ptn ->rc;
        }
    }
    if(_hot->val > v){
        ptn = _hot;
    } else {
        ptn = _hot->right_Node();
    }
    return(iterator(ptn));
}

template <typename T>
    typename
redblacktree<T>::iterator redblacktree<T>::lower_bound(T v){
    RB_Node* ptn = _root;
    _hot = nullptr;
    while(ptn) {
        _hot =ptn;
        if(ptn->val >= v) {
            ptn = ptn ->lc;
        } else {
            ptn = ptn ->rc;
        }
    }
    if(_hot->val >= v){
        ptn = _hot;
    } else {
        ptn = _hot->right_Node();
    }
    return(iterator(ptn));
}

template <typename T>
void redblacktree<T>::init(T v){
    _root = new RB_Node(v,RB_COLOR_BLACK);
    _size = 1;
}

template <typename T>
    typename
redblacktree<T>::iterator redblacktree<T>::insert(T v){
    RB_Node* ptn = find(v);
    if(ptn) {
        return iterator(ptn);
    }
    if(!_hot) {
        //assert(_size ==0)
        init(v);
        return iterator(_root);
    }
    ++_size;
    ptn = new RB_Node(v,RB_COLOR_RED,_hot,nullptr,nullptr);
    if(_hot->val < v){
        _hot->rc = ptn;
    } else {
        _hot->lc = ptn;
    }
    SolveDoubleRed(ptn);
    return iterator(ptn);
}

template <typename T>
void redblacktree<T>::SolveDoubleRed(RB_Node* nn){
    /*
    双红修正3种情况：
    1:如果父亲节点是黑色，无需修正
    2:如果叔叔是红色的 将父亲和叔叔染黑 将祖父染红 上升两层
    3:叔叔是黑色或者不存在(此时还没有上溢) ，把祖父染红父亲染黑，并旋转使得祖父为父亲的儿子，
    */
    while(nn->ftr && nn->ftr->RBc == RB_COLOR_RED){  //排除递归到根和RR-0的情况
        RB_Node* pftr = nn->ftr;
        RB_Node* uncle = bro(pftr);
        RB_Node* grdftr = pftr->ftr;
        if(uncle&&uncle->RBc == RB_COLOR_RED){   //RR-2
             grdftr->RBc =RB_COLOR_RED;
             pftr->RBc =RB_COLOR_BLACK;
             uncle->RBc =RB_COLOR_BLACK;
             nn=grdftr;
        } else {                                 //RR-1  父亲红 祖父黑 叔叔黑（如果父亲是叶子节点，其实叔叔就是NULL，但是存在上溢的时候）
            if(grdftr->lc==pftr) {
                if(pftr->lc == nn) {//同向
                    if(grdftr == _root) {//维护根节点
                        _root = pftr;
                    }
                    zig(grdftr);//右转祖父节点，并且染红祖父节点，染黑父亲节点
                    pftr->RBc = RB_COLOR_BLACK;
                    grdftr->RBc = RB_COLOR_RED;
                } else {
                    if(grdftr == _root) {
                        _root = nn;
                    }
                    zag(pftr);zig(grdftr);
                    nn->RBc = RB_COLOR_BLACK;//插入节点上升两层
                    grdftr->RBc = RB_COLOR_RED;
                }
            } else {
                if(pftr->rc == nn) {
                    if(grdftr == _root) {
                        _root = pftr;
                    }
                    zag(grdftr);
                    pftr->RBc = RB_COLOR_BLACK;
                    grdftr->RBc = RB_COLOR_RED;
                } else {
                    if(grdftr == _root) {
                        _root = nn;
                    }
                    zig(pftr);zag(grdftr);
                    nn->RBc = RB_COLOR_BLACK;
                    grdftr->RBc = RB_COLOR_RED;
                }
            }
            return;
        }
    }
    //RR-2双红修正一定可以结束，递归到根节点或者中途触发RR-0或者RR-1结束
    if(nn ==_root) {                                            //RR-3
        nn->RBc =RB_COLOR_BLACK;
    }
}

template <typename T>
    typename
redblacktree<T>::RB_Node* redblacktree<T>::zig(RB_Node* ptn){
    ptn->lc->ftr = ptn->ftr;
    if(ptn->ftr) {
        if(ptn->ftr->lc == ptn){
            ptn->ftr->lc = ptn->lc;
        } else {
            ptn->ftr->rc = ptn->lc;
        }
    }
    if(ptn->lc->rc) {
        ptn->lc->rc->ftr = ptn;
    }
    ptn->ftr = ptn->lc;
    ptn->lc = ptn->lc->rc;
    ptn->ftr->rc = ptn;
    return ptn->ftr;
}

template <typename T>
    typename
redblacktree<T>::RB_Node* redblacktree<T>::zag(RB_Node* ptn){
    ptn->rc->ftr = ptn->ftr;
    if(ptn->ftr) {
        if(ptn->ftr->lc == ptn){
            ptn->ftr->lc = ptn->rc;
        } else {
            ptn->ftr->rc = ptn->rc;
        }
    }
    if(ptn->rc->lc) {
        ptn->rc->lc->ftr = ptn;
    }
    ptn->ftr = ptn->rc;
    ptn->rc = ptn->rc->lc;
    ptn->ftr->lc = ptn;
    return ptn->ftr;
}

template <typename T>
    typename
redblacktree<T>::iterator redblacktree<T>::begin() {
    if(_root==nullptr)
        return end();
    RB_Node* ptn=_root;
    int i=0;
    while(ptn->lc){
        ptn=ptn->lc;
    }
    return iterator(ptn);
}

template <typename T>
    typename
redblacktree<T>::iterator redblacktree<T>::end() {//类外定义不许用static修饰
    return iterator(NULL);
}

template <typename T>
int redblacktree<T>::size() {
    return _size;
}

template <typename T>
bool redblacktree<T>::empty() {
    return _size==0;
}

template <typename T>
void redblacktree<T>::clear() {
    removetree(_root);
    _size = 0;
    _root = nullptr;
}

template <typename T>
void redblacktree<T>::removetree(RB_Node * ptn){
    if(!ptn)
        return;
    removetree(ptn->lc);
    removetree(ptn->rc);
    delete ptn;
}

template <typename T>
bool redblacktree<T>::remove(T v){
    RB_Node* ptn = find(v);
    RB_Node* node_suc;
    if(!ptn){
        return false;
    }
    --_size;
    while(ptn->lc||ptn->rc){
        //将要删除的节点通过真后继替换的方式移到叶节点
        if(!(ptn->lc)) {
            node_suc = ptn->rc;
        } else if (!ptn->rc) {
            node_suc = ptn->lc;
        } else {
            node_suc =ptn->succ();
        }
        ptn->val = node_suc->val;
        ptn = node_suc;//不维护迭代器
    }
    if(ptn->RBc == RB_COLOR_BLACK) {
        SolveLostBlack(ptn);
    }
    if(ptn->ftr) {
        if(ptn->ftr->lc ==ptn){
            ptn->ftr->lc = nullptr;
        } else {
            ptn->ftr->rc = nullptr;
        }
    }
    if(_root == ptn){
        //assert(_size==0);
        _root = nullptr;
    }
    delete ptn;
    return true;
}

template <typename T>
bool redblacktree<T>::remove(iterator& itr){
    if(!itr._real__node)
        return false;
    RB_Node* ptn = itr._real__node;
    itr._real__node = ptn->right_node();//维护itr
    if(!itr._real__node) {   //没有后继节点就指向前一个？可以指向null
        itr._real__node = ptn->left_Node();
    }
    RB_Node* node_suc;
    --_size;
    while(ptn->lc||ptn->rc){
        if(!(ptn->lc)) {
            node_suc = ptn->rc;
        } else if (!ptn->rc) {
            node_suc = ptn->lc;
        } else {
            node_suc =ptn->succ();
        }
        ptn->val = node_suc->val;
        ptn = node_suc;//不维护迭代器
    }

    if(ptn->RBc == RB_COLOR_BLACK) {
        SolveLostBlack(ptn);
    }
    if(ptn->ftr) {
        if(ptn->ftr->lc ==ptn){
            ptn->ftr->lc = nullptr;
        } else {
            ptn->ftr->rc = nullptr;
        }
    }
    if(_root == ptn) {
        //assert(_size==0);
        _root = nullptr;
    }
    delete ptn;
    return true;
}
/*
    while(nn!=_root){
        RB_Node* pftr = nn->ftr;
        RB_Node* bthr = bro(nn);
        if(bthr->RBc == RB_COLOR_RED){   //LB-1 之后兄弟节点肯定是黑色的 并且父亲是红色的
            bthr->RBc = RB_COLOR_BLACK;
            pftr->RBc = RB_COLOR_RED;
            if(_root==pftr)//旋转过程要维护根节点
                _root = bthr;
            if(pftr->lc == nn) {
                zag(pftr);
            } else {
                zig(pftr);
            }
            bthr = bro(nn);
            pftr = nn->ftr;
        }
        if(bthr->lc && bthr->lc->RBc == RB_COLOR_RED){   //LB-3 兄弟的儿子既然是红色的，兄弟肯定是黑色的
            RB_COLOR oldRBc = pftr->RBc;
            pftr->RBc = RB_COLOR_BLACK;
            //使得兄弟的儿子成为兄弟的父亲，兄弟的父亲成为兄弟的新兄弟
            if(pftr->lc == nn){
                if(_root == pftr){
                    _root =bthr->lc;
                }
                zig(bthr);zag(pftr);
            } else {
                bthr->lc->RBc = RB_COLOR_BLACK;
                if(_root==pftr)
                    _root = bthr;
                zig(pftr);
            }
            pftr->ftr->RBc = oldRBc;
            return;
        } else if(bthr->rc &&bthr->rc->RBc == RB_COLOR_RED){   //LB-3
            RB_COLOR oldRBc = pftr->RBc;
            pftr->RBc = RB_COLOR_BLACK;
            if(pftr->rc == nn){
                if(_root == pftr){
                    _root =bthr->rc;
                }
                zag(bthr);zig(pftr);
            } else {
                bthr->rc->RBc = RB_COLOR_BLACK;
                if(_root==pftr)
                    _root = bthr;
                zag(pftr);
            }
            pftr->ftr->RBc = oldRBc;
            return;
        }

        if(pftr->RBc ==RB_COLOR_RED) {  //LB-2R 父亲红 兄弟黑 且没有红色侄子
            pftr->RBc = RB_COLOR_BLACK;
            bthr->RBc = RB_COLOR_RED;
            return;
        } else {   //LB-2B 父亲黑 兄弟黑 且没有红色侄子  需进行深度logn的递归
            bthr->RBc = RB_COLOR_RED;
            nn=pftr;
        }
    }

*/
template <typename T>
void redblacktree<T>::SolveLostBlack(RB_Node* nn){
    /*
    失黑修正四种情况
    1:红底节点是红色 通过变色转换 将原兄弟改为祖父节点 从而使得父亲节点的颜色变为红色，兄弟节点的颜色变为黑色
    2:1处理后兄弟节点不可能是红色 判断是否有红色侄子
    3:若无只需修改颜色 若有通过变色旋转将侄子旋转到原父亲所在的位置并且他的颜色是原父亲的颜色 将父亲的颜色染黑 从而是的该支路黑高度增加1弥补失黑
    4:全黑递归修改
    */
    while(nn!=_root){
        RB_Node* pftr = nn->ftr;
        RB_Node* bthr = bro(nn);
        if(bthr->RBc == RB_COLOR_RED){//兄弟节点是红色转成父亲节点是红色   LB-1
            pftr->RBc =RB_COLOR_RED;
            bthr->RBc =RB_COLOR_BLACK;
            if(_root==pftr)
                _root = bthr;
            if(nn==pftr->lc)
                zag(pftr);
            else {
                zig(pftr);
            }
            //转动过后nn的亲戚发生了变化
            pftr = nn->ftr;
            bthr = bro(nn);
        }
        if(bthr->lc&&bthr->lc->RBc==RB_COLOR_RED){//失黑修正的话 肯定有兄弟节点 LB-3
            RB_COLOR oldCOL = pftr->RBc;
            pftr->RBc = RB_COLOR_BLACK;
            bthr->lc->RBc=oldCOL;
            if(nn==pftr->lc){//这是异向的 需要旋转两次
                if(pftr==_root)
                    _root=bthr->lc;
                zig(bthr);zag(pftr);
            } else {
                if(pftr==_root)
                    _root=bthr;
                bthr->lc->RBc=RB_COLOR_BLACK;
                zig(pftr);
            }
            return;
        } else if(bthr->rc&&bthr->rc->RBc==RB_COLOR_RED){//失黑修正的话 肯定有兄弟节点 LB-3
            RB_COLOR oldCOL = pftr->RBc;
            pftr->RBc = RB_COLOR_BLACK;
            bthr->rc->RBc=oldCOL;
            if(nn==pftr->rc){//这是异向的 需要旋转两次
                if(pftr==_root)
                    _root=bthr->rc;
                zag(bthr);zig(pftr);
            } else {
                if(pftr==_root)
                    _root=bthr;
                bthr->rc->RBc=RB_COLOR_BLACK;
                zag(pftr);
            }
            return;
        }

        if(pftr->RBc == RB_COLOR_RED){//LB-2R 父亲节点染黑 兄弟染红
            pftr->RBc = RB_COLOR_BLACK;
            bthr->RBc = RB_COLOR_RED;
            return ;
        } else {
            bthr->RBc = RB_COLOR_RED;
            nn=pftr;
        }

    }

}
redblacktree<int> s;
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    srand(time(NULL));
    for(int i(0);i<256;++i){
        s.insert(i);
    }
    for(int i(0);i<256;++i){
        s.remove(i);
    }
    redblacktree<int>::iterator it;
    cout<<"start::"<<endl;
    for(it=s.begin();it!=s.end();++it)
        cout<<*it<<endl;
    cout<<"================================"<<endl;
    s.remove(100);
    for(it=s.begin();it!=s.end();++it)
        cout<<*it<<endl;
    return 0;
}
int bmain(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
