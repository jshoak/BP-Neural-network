#pragma once
#include <iostream>
#include <math.h>
#include <string.h>
#include <fstream>
#include <random>
#include <cmath>
using namespace std;
#define sigmoid(x) (1 / (1 + exp(-(x))))//�����
#define sigmoiddx(x) (x * (1 - (x)))//���������
#define relu(x) ((x < 0) ? 0 : x)//�����
#define reludx(x) ((x < 0) ? 0 : 1)//�����
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

    void train_data(int x, double y, int* number);//ѵ������
    void read_data(const char* s);//��ȡ����
    void write_data(const char* s);//��������

    void initializing_rand();//�����ʼ��Ȩ��ƫ��ֵ
    void initializing_even();//���ȷֲ������ʼ��Ȩ��ƫ��ֵ

    double print();
    int output();//����ʶ������

    void input_layer_expected(double* nonumber, double* exnumber);//���������,�������������ֵ
    /*tanh�����*/
    void forward_propagation_tanh();//����ཻ����ǰ�򴫲�
    void back_propagation_tanh();//���򴫲�
    /*relu�����*/
    void forward_propagation_relu();//����ཻ����ǰ�򴫲�
    void back_propagation_relu();//���򴫲�
    /*sigmoid�����������*/
    void forward_propagation_sigmoid();//ǰ�򴫲�
    void back_propagation_sigmoid();//���򴫲�

    void output_max();//Ѱ����������ֵ
    void soft_max();//ת���ɸ��ʷֲ�
    int maxout();
    double loss();//��ʧ����
private:
    double n;//ѧϰ����
    int number_layers;//�ܹ��������������������
    int* number_nodes;//ÿ��ڵ�����
    double** nodes;//�ڵ�ֵ
    double*** weight;//Ȩ��
    double** offsets;//ƫ��
    double* expected;//����ֵ�����ֵ
    double outmax;//��������ֵ
    double softmax;
    double* pass1;//�ۼƴ������
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
        printf("%d)ʵ����� = %.10f  Ӧ��� = %.10f  ��� = %.10f   \n",
            i, nodes[number_layers - 1][i], expected[i], expected[i] - nodes[number_layers - 1][i]);
    }
    printf("�����  = %.10f      %.10f\n", number, number1);
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
    printf("ʶ������Ϊ��%d�����ƶ�%f%%\n", number, m * 100.0);
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
    for (int i = 0; i < number_nodes[number_layers - 1]; i++)//������
    {
        number += expected[i] * log(nodes[number_layers - 1][i]);
    }
    //printf("������  = %.10f      \n", -number);
    return -number;
}


Neur_network::Neur_network() :n(0), median(0), median_pass(NULL)
, pass1(NULL), pass2(NULL), i(0), j(0), k(0), outmax(0), softmax(0)
{
    number_layers = 0;

    number_nodes = NULL;

    nodes = NULL;//����ÿ������

    weight = NULL;//����Ȩ��

    offsets = NULL;//����ƫ��

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
    number_nodes = new int[number_layers]();//ÿ��ڵ�����
    memcpy(number_nodes, number, sizeof(int) * number_layers);//����ÿ��ڵ�����
    nodes = new double* [number_layers]();//�����ܲ������������������
    for (i = 0; i < number_layers; i++)
    {
        nodes[i] = new double[number_nodes[i]]();//ÿ�㼸���ڵ�
    }
    weight = new double** [number_layers - 1]();//����Ȩ�ز㣬���ܲ�����һ��
    for (i = 0; i < number_layers - 1; i++)
    {
        weight[i] = new double* [number_nodes[i + 1]]();//����ÿ��ڵ���������ȥ�����
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            weight[i][j] = new double[number_nodes[i]]();//ÿ���ڵ���������һ�������
        }
    }
    offsets = new double* [number_layers - 1]();//����ƫ�ã���ȥ�����
    for (i = 0; i < number_layers - 1; i++)
    {
        offsets[i] = new double[number_nodes[i + 1]]();
    }
    expected = new double[number_nodes[number_layers - 1]]();
    pass1 = new double[maxout()]();//�ۼ����
    pass2 = new double[maxout()]();
}
void Neur_network::read_data(const char* s)
{
    ifstream inFile(s, ios::in | ios::binary); //�����ƶ���ʽ��

    inFile.read((char*)&number_layers, sizeof(int));

    number_nodes = new int[number_layers]();
    inFile.read((char*)number_nodes, number_layers * sizeof(int));

    nodes = new double* [number_layers]();//����ÿ������
    for (i = 0; i < number_layers; i++)
    {
        nodes[i] = new double[number_nodes[i]]();
    }
    weight = new double** [number_layers - 1]();//����Ȩ��
    for (i = 0; i < number_layers - 1; i++)
    {
        weight[i] = new double* [number_nodes[i + 1]]();//
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            weight[i][j] = new double[number_nodes[i]]();
            inFile.read((char*)weight[i][j], number_nodes[i] * sizeof(double));
        }
    }
    offsets = new double* [number_layers - 1]();//����ƫ��
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

    outFile.write((char*)&number_layers, sizeof(int));//����

    outFile.write((char*)number_nodes, number_layers * sizeof(int));//ÿ��ڵ�����

    for (i = 0; i < number_layers - 1; i++)
    {
        for (j = 0; j < number_nodes[i + 1]; j++)
        {
            outFile.write((char*)weight[i][j], number_nodes[i] * sizeof(double));//ÿ���ڵ�Ȩ��
        }
    }

    for (i = 0; i < number_layers - 1; i++)
    {
        outFile.write((char*)offsets[i], number_nodes[i + 1] * sizeof(double));//ÿ���ڵ�ƫ��
    }

    outFile.close();
}

void Neur_network::input_layer_expected(double* nonumber, double* exnumber)
{
    memcpy(nodes[0], nonumber, sizeof(double) * number_nodes[0]);//���������
    memcpy(expected, exnumber, sizeof(double) * number_nodes[number_layers - 1]);//����������ֵ
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
    for (i = 1; i < number_layers - 1; i++)//��ȫ����ڶ��㿪ʼ
    {
        for (j = 0; j < number_nodes[i]; j++)//��ǰ�����
        {
            nodes[i][j] = tanh(weight_sum(nodes[i - 1]/*��һ�����*/, 
                weight[i - 1][j], offsets[i - 1][j], number_nodes[i - 1]/*��һ��ڵ�����*/));
        }
    }
    output_max();
    soft_max();
}
void Neur_network::back_propagation_tanh()
{
    for (i = 0; i < number_nodes[number_layers - 1]; i++)
    {
        pass1[i] = (nodes[number_layers - 1][i] - expected[i]);//��ʧ�������������
    }
    /*��ǰ���ǰһ���ƫ��*/
    for (i = number_layers - 1; i > 0; i--)//�ڼ���
    {
        if (i > 1)
        {
            for (j = 0; j < number_nodes[i - 1]; j++)//��ǰ����һ��Ľڵ�����
            {
                median = 0;
                for (k = 0; k < number_nodes[i]; k++)//��ǰ��ڼ����ڵ�
                {
                    median += pass1[k] * weight[i - 1][k][j];
                }
                pass2[j] = median * (1 - tanh(nodes[i - 1][j]) * tanh(nodes[i - 1][j]));// / number_nodes[i];
            }
        }

        for (j = 0; j < number_nodes[i]; j++)//�ڼ����ڵ�
        {
            for (k = 0; k < number_nodes[i - 1]; k++)//��ǰ����������һ����������ڼ���Ȩ��
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
    for (i = number_layers - 1; i > 0; i--)//�ڼ���
    {
        if (i > 1)
        {
            for (j = 0; j < number_nodes[i - 1]; j++)//��ǰ����һ��Ľڵ�����
            {
                median = 0;
                for (k = 0; k < number_nodes[i]; k++)//��ǰ��ڼ����ڵ�
                {
                    median += pass1[k] * weight[i - 1][k][j];
                }
                pass2[j] = median * reludx(nodes[i - 1][j]);// / number_nodes[i];
            }
        }

        for (j = 0; j < number_nodes[i]; j++)//�ڼ����ڵ�
        {
            for (k = 0; k < number_nodes[i - 1]; k++)//��ǰ����������һ����������ڼ���Ȩ��
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
    for (i = number_layers - 1; i > 0; i--)//�ڼ���
    {
        if (i > 1)
        {
            for (j = 0; j < number_nodes[i - 1]; j++)//��ǰ����һ��Ľڵ�����
            {
                median = 0;
                for (k = 0; k < number_nodes[i]; k++)//��ǰ��ڼ����ڵ�
                {
                    median += pass1[k] * weight[i - 1][k][j];
                }
                pass2[j] = median * sigmoiddx(nodes[i - 1][j]);// / number_nodes[i];
            }
        }

        for (j = 0; j < number_nodes[i]; j++)//�ڼ����ڵ�
        {
            for (k = 0; k < number_nodes[i - 1]; k++)//��ǰ����������һ����������ڼ���Ȩ��
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