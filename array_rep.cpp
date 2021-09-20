#include<iostream>
#include<fstream>
#include<chrono>

#define DEBUG
using namespace std;

template<class T>
struct node {
    int r, c;
    T val;

    public:node() {}
    public:node(int r, int c, T val): r(r), c(c), val(val) {}
    int comp(const node<T> &nd) {
        if(this->r < nd.r) {
            return -1;
        } else if(this->r == nd.r) {
            if(this->c < nd.c)
                return -1;
            else if(this->c == nd.c)
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

template<class T>
class sparse_matrix_2d {
    node<T> *arr;
    int len;
    int r, c;

    public:sparse_matrix_2d(int n, int r, int c) {
        if(r<0 || c<0) {
            cout<<"Invalid dimensions\n";
            return;
        }
        len = n;
        arr = new node<T>[n];
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
        this->len = 0;
        for(int i=0; i<r; ++i)
            for(int j=0; j<c; ++j)
                if(mat[i][j] != 0) len++;
        
        arr = new node<T>[len];

        int k = 0;
        for(int i=0; i<r; ++i) {
            for(int j=0; j<c; ++j) {
                if(mat[i][j] != 0) {
                    arr[k].r = i;
                    arr[k].c = j;
                    arr[k].val = mat[i][j];
                    k++;
                }
            }
        }
    }

    public:sparse_matrix_2d(const sparse_matrix_2d &mat) {
        *this = mat;
    }

    public:void operator=(const sparse_matrix_2d &mat) {
        this->len = mat.len;
        this->r = mat.r;
        this->c = mat.c;
        this->arr = new node<T>[this->len];
        for(int i=0; i<len; ++i)
            arr[i] = mat.arr[i];
    }

    ~sparse_matrix_2d() {
        delete[] arr;
    }

    public:sparse_matrix_2d operator+(const sparse_matrix_2d &mat) {
        if(this->r != mat.r || this->c != mat.c) {
            cout<<"Invalid dimensions for addition\n";
            return sparse_matrix_2d(0, 0, 0);
        }

        int i=0, j=0;
        int max_len = this->len + mat.len;
        int k = 0;
        sparse_matrix_2d sum(max_len, this->r, this->c);
        while(i<this->len && j<mat.len) {
            if(arr[i] == mat.arr[j]) {
                sum.arr[k].r = arr[i].r;
                sum.arr[k].c = arr[i].c;
                sum.arr[k].val = arr[i].val + mat.arr[j].val;
                i++;
                j++;
            } else if(arr[i] < mat.arr[j]) {
                sum.arr[k].r = arr[i].r;
                sum.arr[k].c = arr[i].c;
                sum.arr[k].val = arr[i].val;
                i++;
            } else {
                sum.arr[k].r = mat.arr[j].r;
                sum.arr[k].c = mat.arr[j].c;
                sum.arr[k].val = mat.arr[j].val;
                j++;
            }
            if(sum.arr[k].val!=0) k++;
        }
        while(i < this->len) {
            sum.arr[k].r = arr[i].r;
            sum.arr[k].c = arr[i].c;
            sum.arr[k++].val = arr[i++].val;
        }
        while(j < mat.len) {
                sum.arr[k].r = mat.arr[j].r;
                sum.arr[k].c = mat.arr[j].c;
                sum.arr[k++].val = mat.arr[j++].val;        
        }
        sum.len = k;

        return sum;
    }

    public:sparse_matrix_2d transpose() const {
        sparse_matrix_2d t(len, r, c);
        int *col_count = new int[c]();
        for(int i=0; i<len; ++i)
            col_count[arr[i].c]++;
        
        int *index = new int[c]();
        index[0] = 0;
        for(int i=1; i<c; ++i)
            index[i] = index[i-1] + col_count[i-1];
        
        for(int i=0; i<len; ++i)
            t.arr[index[arr[i].c]++] = node<T>(arr[i].c, arr[i].r, arr[i].val);
        
        return t;
    }

    public:sparse_matrix_2d operator*(const sparse_matrix_2d &mat) {
        if(this->c != mat.r) {
            cout<<"invalid matrix dimensions\n";
            return sparse_matrix_2d<T>(0, 0, 0);
        }
        
        sparse_matrix_2d matT = mat.transpose();
        sparse_matrix_2d res(r*mat.c, r, mat.c);
        int k = 0;
        int i1 = 0;
        while(i1 < len) {
            int res_r = arr[i1].r;
            int i2 = 0;
            while(i2 < matT.len) {
                int res_c = matT.arr[i2].r;
                int ti1 = i1, ti2 = i2;
                T sum = 0;
                while(ti1 < len && arr[ti1].r == res_r && ti2 < matT.len && matT.arr[ti2].r == res_c) {
                    if(arr[ti1].c == matT.arr[ti2].c)
                        sum += arr[ti1++].val * matT.arr[ti2++].val;
                    else if(arr[ti1].c < matT.arr[ti2].c)
                        ++ti1;
                    else
                        ++ti2;
                }
                if(sum != 0) {
                    res.arr[k].r = res_r;
                    res.arr[k].c = res_c;
                    res.arr[k++].val = sum;
                }
                
                while(i2 < matT.len && matT.arr[i2].r == res_c)
                    ++i2;
            }
            while(i1 < len && arr[i1].r == res_r)
                ++i1;
        }
        res.len = k;

        return res;
    }

    public:void display() {
        for(int i=0; i<len; ++i)
            cout<<arr[i].r<<" "<<arr[i].c<<" "<<arr[i].val<<"\n";
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