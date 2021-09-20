#include<iostream>
#include<fstream>
#include<chrono>

#define DEBUG

using namespace std;

template<class T>
struct node {
    int i, j;
    T val;
    node<T> *next;
    node<T> *prev;

    public:node() {}
    public:node(int i, int j, T val): i(i), j(j), val(val) {}
    int comp(const node<T> &nd) {
        if(this->i < nd.i) {
            return -1;
        } else if(this->i == nd.i) {
            if(this->j < nd.j)
                return -1;
            else if(this->j == nd.j)
                return 0;
            else 
                return 1;
        } else {
            return 1;
        }
    }

    bool operator<(const node<T> &nd) {
        return comp(nd) < 0;
    }

    bool operator>(const node<T> &nd) {
        return comp(nd) > 0;
    }

    bool operator==(const node<T> &nd) {
        return comp(nd) == 0;
    }
};

template<typename T>
class list {
    node<T> *_front;
    node<T> *_back;
    int _size;

    public:list(): _front(nullptr), _back(nullptr), _size(0) {}

    public:void push_back(node<T> *nd) {
        if(nd == nullptr)
            return;
        if(empty()) {
            _size++;
            _back = nd;
            _front = nd;
            return;
        }

        insertAfter(_back, nd);
    }

    public:void push_front(node<T> *nd) {
        if(nd == nullptr)
            return;
        if(empty()) {
            _size++;
            _back = nd;
            _front = nd;
            return;
        }

        insertBefore(_front, nd);
    }

    public:node<T> *detach(node<T> *nd) {
        if(nd == nullptr)
            return nullptr;
        
        node<T> *prev = nd->prev;
        node<T> *next = nd->next;

        if(prev!=nullptr) prev->next = next;
        else _front = next;
        if(next!=nullptr) next->prev = prev;
        else _back = prev;

        nd->next = nullptr;
        nd->prev = nullptr;
        --_size;

        return nd;
    }

    public:void insertAfter(node<T> *nd, node<T> *new_node) {
        if(nd == nullptr || new_node == nullptr)
            return;
        ++_size;
        new_node->next = nd->next;
        new_node->prev = nd;
        if(nd->next != nullptr) nd->next->prev = new_node;
        nd->next = new_node;

        if(new_node->next == nullptr)
            _back = new_node;
    }

    public:void insertBefore(node<T> *nd, node<T> *new_node) {
        if(nd == nullptr || new_node == nullptr)
            return;
        ++_size;
        new_node->next = nd;
        new_node->prev = nd->prev;
        if(nd->prev != nullptr) nd->prev->next = new_node;
        nd->prev = new_node;

        if(new_node->prev == nullptr)
            _front = new_node;
    }

    public:node<T> *pop_front() {
        return detach(_front);
    }

    public:node<T> *pop_back() {
        return detach(_back);
    }

    public:bool empty() const {
        return _back == nullptr;
    }

    public:int size() const {
        return _size;
    }

    public:node<T> *front() const {
        return _front;
    }

    public:node<T> *back() const {
        return _back;
    }
};

template<typename T>
class sparse_matrix_2d {
    list<T> li;
    int r, c;

    public:sparse_matrix_2d(int r, int c) {
        if(r<0 || c<0) {
            cout<<"Invalid dimensions\n";
            return;
        }
        this->r = r;
        this->c = c;
    }
    public:sparse_matrix_2d(T **mat, int r, int c) {
        if(r<0 || c<0 || mat==nullptr) {
            cout<<"Invalid dimensions\n";
            return;
        }
        this->r = r;
        this->c = c;
        for(int i=0; i<r; ++i)
            for(int j=0; j<c; ++j) 
                if(mat[i][j] != 0)
                    li.push_back(new node<T>(i, j, mat[i][j]));
    }

    public:sparse_matrix_2d(const sparse_matrix_2d<T> &mat) {
        *this = mat;
    }

    public:~sparse_matrix_2d() {
        while(!li.empty()) {
            delete li.pop_front();
        }
    }

    public:void operator=(const sparse_matrix_2d<T> &mat) {
        this->r = mat.r;
        this->c = mat.c;
        node<T> *temp = mat.li.front();
        while(temp!=nullptr) {
            li.push_back(new node<T>(temp->i, temp->j, temp->val));
            temp = temp->next;
        }
    }

    public:int size() const {
        return li.size();
    }

    public:sparse_matrix_2d<T> operator+(const sparse_matrix_2d<T> &mat) {
        if(this->r != mat.r || this->c != mat.c) {
            cout<<"Invalid matrix dimensions\n";
            return sparse_matrix_2d(0, 0);
        }
        node<T> *temp1 = li.front();
        node<T> *temp2 = mat.li.front();
        sparse_matrix_2d sum(this->r, this->c);
        while(temp1!=nullptr && temp2 != nullptr) {
            if(*temp1 == *temp2) {
                sum.li.push_back(new node<T>(temp1->i, temp1->j, temp1->val + temp2->val));
                temp1 = temp1->next;
                temp2 = temp2->next;
            } else if(*temp1 < *temp2) {
                sum.li.push_back(new node<T>(temp1->i, temp1->j, temp1->val));
                temp1 = temp1->next;
            } else {
                sum.li.push_back(new node<T>(temp2->i, temp2->j, temp2->val));
                temp2 = temp2->next;
            }
            if(sum.li.back()->val == 0)
                delete sum.li.pop_back();
        }
        for(;temp1 != nullptr; temp1 = temp1->next)
            sum.li.push_back(new node<T>(temp1->i, temp1->j, temp1->val));

        for(;temp2 != nullptr; temp2 = temp2->next)
            sum.li.push_back(new node<T>(temp2->i, temp2->j, temp2->val));

        return sum;
    }

    public:sparse_matrix_2d<T> transpose() const {
        int size = this->li.size();
        /* create new matrix to store transpose */
        sparse_matrix_2d<T> trans(this->r, this->c);
        for(int i=0; i<size; ++i)
            trans.li.push_back(new node<T>(-1, -1, -1));
        
        /* array to store the address of nodes */
        node<T> **ptr_arr = new node<T>*[size];
        /* array to store the count of column elements */
        int *count = new int[this->c]();
        /* array to store correct position of elements in transpose */
        int *index = new int[this->c]();

        /* fill the arrays */
        node<T> *temp1 = this->li.front();
        node<T> *temp2 = trans.li.front();
        for(int i=0; i<size; ++i) {
            ptr_arr[i] = temp2;
            ++count[temp1->j];
            temp1 = temp1->next;
            temp2 = temp2->next;
        }

        /* calculate correct position for each node */
        index[0] = 0;
        for(int i=1; i<this->c; ++i)
            index[i] = index[i-1] + count[i-1];

        /* tranpose the original matrix */
        node<T> *temp = this->li.front();
        while(temp!=nullptr) {
            node<T> *ptr = ptr_arr[index[temp->j]++];
            ptr->i = temp->j;
            ptr->j = temp->i;
            ptr->val = temp->val;
            temp = temp->next;
        }

        for(int i=0; i<size; ++i)
            ptr_arr[i] = nullptr;
        delete[] ptr_arr;
        delete[] count;
        delete[] index;
        
        return trans;
    }

    public:sparse_matrix_2d<T> operator*(const sparse_matrix_2d<T> &mat) {
        if(this->c != mat.r) {
            cout<<"invalid matrix dimensions\n";
            return sparse_matrix_2d<T>(0, 0);
        }
        sparse_matrix_2d<T> trans = mat.transpose();
        sparse_matrix_2d<T> res(this->r, mat.c);

        
        node<T> *p1 = this->li.front();
        while(p1 != nullptr) {
            int i = p1->i;
            node<T> *p2 = trans.li.front();
            while(p2 != nullptr) {
                int j = p2->i;
                node<T> *tp1 = p1, *tp2 = p2;
                T sum = 0;
                while(tp1!=nullptr && tp1->i == i && tp2!=nullptr && tp2->i == j) {
                    if(tp1->j == tp2->j) {
                        sum += tp1->val * tp2->val;
                        tp1 = tp1->next;
                        tp2 = tp2->next;
                    } else if(tp1->j < tp2->j) {
                        tp1 = tp1->next;
                    } else {
                        tp2 = tp2->next;
                    }
                }
                if(sum != 0)
                    res.li.push_back(new node<T>(i, j, sum));
                while(p2 != nullptr && p2->i == j)
                    p2 = p2->next;
            }
            while(p1 != nullptr && p1->i == i)
                p1 = p1->next;
        }

        return res;
    }

    public:void display() {
        node<T> *temp = li.front();
        while(temp!=nullptr) {
            cout<<temp->i<<" "<<temp->j<<" "<<temp->val<<"\n";
            temp = temp->next;
        }
        cout<<"\n";
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    #ifdef DEBUG
    ifstream _in("input.txt");
    ofstream _out("output.txt");
    streambuf* _in_backup = cin.rdbuf();
    streambuf* _out_backup = cout.rdbuf();
    cin.rdbuf(_in.rdbuf());
    cout.rdbuf(_out.rdbuf());
    auto _start = chrono::high_resolution_clock::now();
    #endif
    /* ######################CODE_START################################ */
    int n1, m1;
    cin>>n1>>m1;
    int **arr1 = new int*[n1];
    for(int i=0; i<n1; ++i)
        arr1[i] = new int[m1];
    for(int i=0; i<n1; ++i)
        for(int j=0; j<m1; ++j)
            cin>>arr1[i][j];
    sparse_matrix_2d<int> mat1(arr1, n1, m1);

    int n2, m2;
    cin>>n2>>m2;
    int **arr2 = new int*[n2];
    for(int i=0; i<n2; ++i)
        arr2[i] = new int[m2];
    for(int i=0; i<n2; ++i)
        for(int j=0; j<m2; ++j)
            cin>>arr2[i][j];
    sparse_matrix_2d<int> mat2(arr2, n2, m2);

    cout<<"sparse matrix 1\n";
    mat1.display();
    cout<<"sparse matrix 2\n";
    mat2.display();

    cout<<"sum\n";
    sparse_matrix_2d<int> sum = mat1 + mat2;
    sum.display();

    cout<<"transponse\n";
    sparse_matrix_2d<int> t = mat2.transpose();
    t.display();

    cout<<"product\n";
    sparse_matrix_2d<int> p = mat1 * mat2;
    p.display();

    /* #######################CODE_END############################### */
    #ifdef DEBUG
    auto _end = chrono::high_resolution_clock::now();
    auto _duration = chrono::duration_cast<chrono::nanoseconds>(_end - _start);
    long long _ns = _duration.count();
    double _s = _ns / 1e9;
    cout<<"\nDuration: "<<_s<<" sec\n";
    cin.rdbuf(_in_backup);
    cout.rdbuf(_out_backup);
    #endif
    return 0;
}