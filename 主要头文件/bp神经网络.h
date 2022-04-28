#pragma once
#include <iostream>
#include <math.h>
#include <string.h>
#include <fstream>
#include <random>
#include <cmath>
using namespace std;
#define sigmoid(x) (1 / (1 + exp(-(x))))//激活函数
#define sigmoiddx(x) (x * (1 - (x)))//激活函数导数
#define relu(x) ((x < 0) ? 0 : x)//激活函数
#define reludx(x) ((x < 0) ? 0 : 1)//激活函数
double weight_sum(double* o, double* w, double b, int m)
{
    double h = 0;

    for (int i = 0; i < m; i++)
    {
        h += o[i] * w[i];
    }
    return h + b;
}
class Neur_network
{

public:
    Neur_network();
    ~Neur_network();

    void train_data(int x, double y, int* number);//训练数据
    void read_data(const char* s);//读取数据
    void write_data(const char* s);//保存数据

    void initializing_rand();//随机初始化权重偏置值
    void initializing_even();//均匀分布随机初始化权重偏置值

    double print();
    int output();//返回识别数字

    void input_layer_expected(double* nonumber, double* exnumber);//设置输入层,设置期望的输出值
    /*tanh激活函数*/
    void forward_propagation_tanh();//多分类交叉熵前向传播
    void back_propagation_tanh();//反向传播
    /*relu激活函数*/
    void forward_propagation_relu();//多分类交叉熵前向传播
    void back_propagation_relu();//反向传播
    /*sigmoid激活函数均方差*/
    void forward_propagation_sigmoid();//前向传播
    void back_propagation_sigmoid();//反向传播

    void output_max();//寻找输出层最大值
    void soft_max();//转换成概率分布
    int maxout();
    double loss();//损失函数
private:
    double n;//学习速率
    int number_layers;//总共层数，包含输入输出层
    int* number_nodes;//每层节点数量
    double** nodes;//节点值
    double*** weight;//权重
    double** offsets;//偏置
    double* expected;//期望值的输出值
    double outmax;//输出层最大值
    double softmax;
    double* pass1;//累计传递误差
    double* pass2;
    double* median_pass;
    double median;
    int i;
    int j;
    int k;
};

double Neur_network::print()
{
    double number = 0;
    double number1 = 0;
    for (int i = 0; i < number_nodes[number_layers - 1]; i++)
    {
        number += fabs(expected[i] - nodes[number_layers - 1][i]);
        number1 += nodes[number_layers - 1][i];
        printf("%d)实际输出 = %.10f  应输出 = %.10f  误差 = %.10f   \n",
            i, nodes[number_layers - 1][i], expected[i], expected[i] - nodes[number_layers - 1][i]);
    }
    printf("总误差  = %.10f      %.10f\n", number, number1);
    return number;
}
int Neur_network::output()
{
    int number = 0;
    double m = 0;
    for (int i = 0; i < number_nodes[number_layers - 1]; i++)
    {
        if (nodes[number_layers - 1][i] > m)
        {
            m = nodes[number_layers - 1][i];
            number = i;
        }
    }
    printf("识别数字为：%d，相似度%f%%\n", number, m * 100.0);
    return number;
}

void Neur_network::output_max()
{
    outmax = 0;
    for (j = 0; j < number_nodes[number_layers - 1]; j++)
    {
        nodes[number_layers - 1][j] = (weight_sum(nodes[number_layers - 2], weight[number_layers - 2][j], offsets[number_layers - 2][j], number_nodes[number_layers - 2]));
        if (nodes[number_layers - 1][j] > outmax)outmax = nodes[number_layers - 1][j];
    }
}
void Neur_network::soft_max()
{
    softmax = 0;
    for (j = 0; j < number_nodes[number_layers - 1]; j++)
    {
        nodes[number_layers - 1][j] = exp(nodes[number_layers - 1][j] - outmax);
        softmax += nodes[number_layers - 1][j];
    }
    for (j = 0; j < number_nodes[number_layers - 1]; j++)
    {
        nodes[number_layers - 1][j] = nodes[number_layers - 1][j] / softmax;
    }
}
int Neur_network::maxout()
{
    int n = 0;
    for (int i = 1; i < number_layers; i++)
        if (number_nodes[i] > n)n = number_nodes[i];
    return n;
}
double Neur_network::loss()
{
    double number = 0;
    for (int i = 0; i < number_nodes[number_layers - 1]; i++)//交叉熵
    {
        number += expected[i] * log(nodes[number_layers - 1][i]);
    }
    //printf("交叉熵  = %.10f      \n", -number);
    return -number;
}


Neur_network::Neur_network() :n(0), median(0), median_pass(NULL)
, pass1(NULL), pass2(NULL), i(0), j(0), k(0), outmax(0), softmax(0)
{
    number_layers = 0;

    number_nodes = NULL;

    nodes = NULL;//分配每层数量

    weight = NULL;//分配权重

    offsets = NULL;//分配偏置

    expected = NULL;
}
Neur_network::~Neur_network()
{
    for (i = 0; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            delete[] weight[i][j];
        }
        delete[] weight[i];
    }
    delete[] weight;

    for (i = 0; i < number_layers; i++)
    {
        delete[] nodes[i];
    }
    delete[] nodes;

    for (i = 0; i < number_layers - 1; i++)
    {
        delete[] offsets[i];
    }
    delete[] offsets;

    delete[] number_nodes;

    delete[] expected;

    delete[] pass1;

    delete[] pass2;
}

void Neur_network::train_data(int x, double y, int* number)
{
    number_layers = x;
    n = y;
    number_nodes = new int[number_layers]();//每层节点数量
    memcpy(number_nodes, number, sizeof(int) * number_layers);//设置每层节点数量
    nodes = new double* [number_layers]();//分配总层数，包含输入输出层
    for (i = 0; i < number_layers; i++)
    {
        nodes[i] = new double[number_nodes[i]]();//每层几个节点
    }
    weight = new double** [number_layers - 1]();//分配权重层，比总层数少一层
    for (i = 0; i < number_layers - 1; i++)
    {
        weight[i] = new double* [number_nodes[i + 1]]();//分配每层节点数量，除去输出层
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            weight[i][j] = new double[number_nodes[i]]();//每个节点所连接上一层的数量
        }
    }
    offsets = new double* [number_layers - 1]();//分配偏置，除去输出层
    for (i = 0; i < number_layers - 1; i++)
    {
        offsets[i] = new double[number_nodes[i + 1]]();
    }
    expected = new double[number_nodes[number_layers - 1]]();
    pass1 = new double[maxout()]();//累计误差
    pass2 = new double[maxout()]();
}
void Neur_network::read_data(const char* s)
{
    ifstream inFile(s, ios::in | ios::binary); //二进制读方式打开

    inFile.read((char*)&number_layers, sizeof(int));

    number_nodes = new int[number_layers]();
    inFile.read((char*)number_nodes, number_layers * sizeof(int));

    nodes = new double* [number_layers]();//分配每层数量
    for (i = 0; i < number_layers; i++)
    {
        nodes[i] = new double[number_nodes[i]]();
    }
    weight = new double** [number_layers - 1]();//分配权重
    for (i = 0; i < number_layers - 1; i++)
    {
        weight[i] = new double* [number_nodes[i + 1]]();//
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            weight[i][j] = new double[number_nodes[i]]();
            inFile.read((char*)weight[i][j], number_nodes[i] * sizeof(double));
        }
    }
    offsets = new double* [number_layers - 1]();//分配偏置
    for (i = 0; i < number_layers - 1; i++)
    {
        offsets[i] = new double[number_nodes[i + 1]]();
        inFile.read((char*)offsets[i], number_nodes[i + 1] * sizeof(double));
    }

    expected = new double[number_nodes[number_layers - 1]]();

    inFile.close();
}
void Neur_network::write_data(const char* s)
{
    ofstream outFile(s, ios::out | ios::binary);

    outFile.write((char*)&number_layers, sizeof(int));//层数

    outFile.write((char*)number_nodes, number_layers * sizeof(int));//每层节点数量

    for (i = 0; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            outFile.write((char*)weight[i][j], number_nodes[i] * sizeof(double));//每个节点权重
        }
    }

    for (i = 0; i < number_layers - 1; i++)
    {
        outFile.write((char*)offsets[i], number_nodes[i + 1] * sizeof(double));//每个节点偏置
    }

    outFile.close();
}

void Neur_network::input_layer_expected(double* nonumber, double* exnumber)
{
    memcpy(nodes[0], nonumber, sizeof(double) * number_nodes[0]);//设置输入层
    memcpy(expected, exnumber, sizeof(double) * number_nodes[number_layers - 1]);//设置期望的值
}

void Neur_network::initializing_even()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> d(-sqrt(6.0 / number_nodes[0]), sqrt(6.0 / number_nodes[0]));
    for (i = 0; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            for (k = 0; k < number_nodes[i]; k++)
            {
                weight[i][j][k] = d(gen);
            }
        }
    }

    for (i = 0; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            offsets[i][j] = d(gen);
        }
    }
}
void Neur_network::initializing_rand()
{

    srand((unsigned)time(NULL));
    for (i = 0; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            for (k = 0; k < number_nodes[i]; k++)
            {
                //weight[i][j][k] = rand() / 32767.0;
                weight[i][j][k] = (rand() - 16383.5) / 16383.50;
            }
        }
    }

    for (i = 0; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            //offsets[i][j] = rand() / 32767.0;
            offsets[i][j] = (rand() - 16383.5) / 16383.50;
        }
    }
}

void Neur_network::forward_propagation_tanh()
{
    for (i = 1; i < number_layers - 1; i++)//从全部层第二层开始
    {
        for (j = 0; j < number_nodes[i]; j++)//当前层层数
        {
            nodes[i][j] = tanh(weight_sum(nodes[i - 1]/*上一层输出*/, 
                weight[i - 1][j], offsets[i - 1][j], number_nodes[i - 1]/*上一层节点数量*/));
        }
    }
    output_max();
    soft_max();
}
void Neur_network::back_propagation_tanh()
{
    for (i = 0; i < number_nodes[number_layers - 1]; i++)
    {
        pass1[i] = (nodes[number_layers - 1][i] - expected[i]);//损失函数至输入层求导
    }
    /*提前算出前一层的偏导*/
    for (i = number_layers - 1; i > 0; i--)//第几层
    {
        if (i > 1)
        {
            for (j = 0; j < number_nodes[i - 1]; j++)//当前层上一层的节点数量
            {
                median = 0;
                for (k = 0; k < number_nodes[i]; k++)//当前层第几个节点
                {
                    median += pass1[k] * weight[i - 1][k][j];
                }
                pass2[j] = median * (1 - tanh(nodes[i - 1][j]) * tanh(nodes[i - 1][j]));// / number_nodes[i];
            }
        }

        for (j = 0; j < number_nodes[i]; j++)//第几个节点
        {
            for (k = 0; k < number_nodes[i - 1]; k++)//当前层所连接上一层的数量，第几个权重
            {
                weight[i - 1][j][k] -= n * pass1[j] * nodes[i - 1][k];
            }
            offsets[i - 1][j] -= n * pass1[j];
        }

        median_pass = pass1;
        pass1 = pass2;
        pass2 = median_pass;
    }
}

void Neur_network::forward_propagation_relu()
{
    for (i = 1; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i]; j++)
        {
            nodes[i][j] = relu(weight_sum(nodes[i - 1], weight[i - 1][j], offsets[i - 1][j], number_nodes[i - 1]));
        }
    }

    output_max();

    soft_max();
}
void Neur_network::back_propagation_relu()
{
    for (i = 0; i < number_nodes[number_layers - 1]; i++)
    {
        pass1[i] = (nodes[number_layers - 1][i] - expected[i]);
    }
    for (i = number_layers - 1; i > 0; i--)//第几层
    {
        if (i > 1)
        {
            for (j = 0; j < number_nodes[i - 1]; j++)//当前层上一层的节点数量
            {
                median = 0;
                for (k = 0; k < number_nodes[i]; k++)//当前层第几个节点
                {
                    median += pass1[k] * weight[i - 1][k][j];
                }
                pass2[j] = median * reludx(nodes[i - 1][j]);// / number_nodes[i];
            }
        }

        for (j = 0; j < number_nodes[i]; j++)//第几个节点
        {
            for (k = 0; k < number_nodes[i - 1]; k++)//当前层所连接上一层的数量，第几个权重
            {
                weight[i - 1][j][k] -= n * pass1[j] * nodes[i - 1][k];
            }
            offsets[i - 1][j] -= n * pass1[j];
        }
        median_pass = pass1;
        pass1 = pass2;
        pass2 = median_pass;
    }
}

void Neur_network::forward_propagation_sigmoid()
{
    for (i = 1; i < number_layers; i++)
    {
        for (j = 0; j < number_nodes[i]; j++)
        {
            nodes[i][j] = sigmoid(weight_sum(nodes[i - 1], weight[i - 1][j], offsets[i - 1][j], number_nodes[i - 1]));
        }
    }
}
void Neur_network::back_propagation_sigmoid()
{
    for (i = 0; i < number_nodes[number_layers - 1]; i++)
    {
        pass1[i] = -(expected[i] - nodes[number_layers - 1][i]) * sigmoiddx(nodes[number_layers - 1][i]);
    }
    for (i = number_layers - 1; i > 0; i--)//第几层
    {
        if (i > 1)
        {
            for (j = 0; j < number_nodes[i - 1]; j++)//当前层上一层的节点数量
            {
                median = 0;
                for (k = 0; k < number_nodes[i]; k++)//当前层第几个节点
                {
                    median += pass1[k] * weight[i - 1][k][j];
                }
                pass2[j] = median * sigmoiddx(nodes[i - 1][j]);// / number_nodes[i];
            }
        }

        for (j = 0; j < number_nodes[i]; j++)//第几个节点
        {
            for (k = 0; k < number_nodes[i - 1]; k++)//当前层所连接上一层的数量，第几个权重
            {
                weight[i - 1][j][k] -= n * pass1[j] * nodes[i - 1][k];
            }
            offsets[i - 1][j] -= n * pass1[j];
        }
        median_pass = pass1;
        pass1 = pass2;
        pass2 = median_pass;
    }
}