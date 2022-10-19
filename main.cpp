#include <iostream>
#include <vector>
#include "tinyxml2.h"
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace tinyxml2;

class Class1 {
public:
    int obraz;
    vector<vector<float>> coord; // координата образа
    vector<vector<float>> rasnost_coord; // разность координат нового объекта и ядра класса
    vector<vector<float>> kovariaz_umnog; // (x-y)^T * (S+E)^(-1)
    vector<vector<float>> kovariaz_matrix; // матрица ковариации. в результате будет иметь вид (S+E)^-1
    vector<vector<float>> core; // ядро класса
    float final; // 
    Class1() {};
    ~Class1() {};
};

//Вычисляем (S+E)^-1
void E_M(vector<vector<float>>& M) {
    double t;
    vector<vector<float>> E(M.size(), vector<float>(M.size(), 0.0));
    //определяем единичную матрицу и складываем с матрицей ковариации
    for (int i = 0; i < M.size(); i++)
        for (int j = 0; j < M.size(); j++)
        {
            if (i == j) { E[i][j] = 1.0; }
            M[i][j] += E[i][j];
        }

    //Задаём номер ведущей строки
    for (int a = 0; a < M.size(); a++)
    {

        t = M[a][a];
        //все элементы a-ой строки матрицы M, кроме a-ого и до него, делим на разрешающий элемент
        for (int b = a + 1; b < M.size(); b++)
        {
            M[a][b] = M[a][b] / t;
        }
        //все элементы a-ой строки матрицы E делим на разрешающий элемент
        for (int b = 0; b < M.size();b++) {
            E[a][b] = E[a][b] / t;
        }
        //элемент соответствующий  разрещающему - делим на самого себя, чтобы получить 1
        M[a][a] /= t;

        if (a > 0) {
            for (int i = 0;i < a;i++) {
                for (int j = 0;j < M.size();j++) {
                    E[i][j] = E[i][j] - E[a][j] * M[i][a]; //Вычисляем элементы матрицы E,идя по столбцам с 0 -ого  к последнему
                }
                for (int j = M.size() - 1;j >= a;j--) {
                    M[i][j] = M[i][j] - M[a][j] * M[i][a]; //Вычисляем элементы матрицы M,идя по столбцам с последнего к a-ому
                }
            }
        }
        for (int i = a + 1;i < M.size();i++) {
            for (int j = 0;j < M.size();j++) {
                E[i][j] = E[i][j] - E[a][j] * M[i][a];
            }
            for (int j = M.size() - 1;j >= a;j--) {
                M[i][j] = M[i][j] - M[a][j] * M[i][a];
            }
        }

    }
    //На месте исходной матрицы должна получиться единичная а на месте единичной - обратная.
    for (int i = 0; i < M.size(); i++) {
        for (int b = 0; b < M.size(); b++) {
            M[i][b] = E[i][b];
        }
    }
}

void print_line() {
    for (int m = 0;m < 100;m++)
        cout << "_";
    cout << endl;
}

class Loader
{
public:
    std::vector<std::vector<std::vector<int>>> examples;
    std::vector<string> classes; //каждый элементы вектора - id класса сответсвующего образца
    std::vector<std::vector<std::vector<int>>> tasks;
    bool load_instance(const char* fileName)
    {
        XMLDocument doc;
        if(doc.LoadFile(fileName) != XMLError::XML_SUCCESS)
        {
            std::cout << "Error openning input XML file."<<std::endl;
            return false;
        }
        XMLElement* root;
        root = doc.FirstChildElement("root");
        XMLElement* objects = root->FirstChildElement("examples");
        for(auto object=objects->FirstChildElement("object"); object; object = object->NextSiblingElement("object"))
        {
            std::vector<std::vector<int>> example;
            classes.push_back(object->Attribute("class"));
            for(auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row"))
            {
                std::vector<int> line;
                std::string values = row->GetText();
                for(char value : values)
                {
                    if(value == '1')
                        line.push_back(1);
                    else if(value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            examples.push_back(example);
        }
        XMLElement* task = root->FirstChildElement("tasks");
        for(auto object=task->FirstChildElement(); object; object = object->NextSiblingElement("object"))
        {
            std::vector<std::vector<int>> example;
            for(auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row"))
            {
                std::vector<int> line;
                std::string values = row->GetText();
                for(char value : values)
                {
                    if(value == '1')
                        line.push_back(1);
                    else if(value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            tasks.push_back(example);
        }
        return true;
    }
    void print_examples()
    {
        for(int i = 0; i < examples.size(); i++)
        {
            std::cout<<"\nObject "<<i<<" class="<<classes[i]<<"\n";
            for(int j=0; j<examples[i].size(); j++) {
                for (int k = 0; k < examples[i][j].size(); k++) {
                    if (examples[i][j][k] == 0) std::cout << "\x0B0";
                    else std::cout << "\x0B2";
                }
                std::cout<<"\n";
            }
        }
    }
    void print_tasks(int i, string class1) {
            std::cout << "\nTask " << i << " supposed class = " << class1 <<"\n";
            for (int j = 0; j < tasks[i].size(); j++) {
                for (int k = 0; k < tasks[i][j].size(); k++)
                    if (tasks[i][j][k] == 0) std::cout << "\x0B0";
                    else std::cout << "\x0B2";
                std::cout << "\n";
            }
    }
};

int main(int argc, char* argv[]) //argc - argumnet counter, argv - argument values
{
    for(int i=0; i<argc; i++)
        std::cout<<argv[i]<<"\n";
    if(argc<2)
    {
        std::cout << "Name of the input XML file is not specified."<<std::endl;
        return 1;
    }
    Loader loader;
    loader.load_instance(argv[1]);
    loader.print_examples();

    int clauses_amount = 4;
    int image_amount = 4;
    int task_image_amount = 4;

    int n = 10, m = 10;
    

    vector<vector<Class1>> oblast(clauses_amount, vector<Class1>(image_amount)); // список всех образов (первый ключ - класс, второй - образ)

    // задаем размер матрицам и обнуляем их
    for (int i = 0;i < clauses_amount;i++) {
        for (int j = 0;j < image_amount;j++) {
            oblast[i][j].coord.resize(n);
            oblast[i][j].core.resize(n);
            oblast[i][j].kovariaz_matrix.resize(n * m);
            oblast[i][j].rasnost_coord.resize(n);
            oblast[i][j].kovariaz_umnog.resize(n);
            oblast[i][j].final = 0;

            for (int i1 = 0;i1 < n;i1++) {
                oblast[i][j].coord[i1].resize(m);
                oblast[i][j].core[i1].resize(m);
                oblast[i][j].rasnost_coord[i1].resize(m);
                oblast[i][j].kovariaz_umnog[i1].resize(m);

                for (int j1 = 0;j1 < m;j1++) {
                    oblast[i][j].coord[i1][j1] = 0;
                    oblast[i][j].core[i1][j1] = 0;
                    oblast[i][j].kovariaz_umnog[i1][j1] = 0;
                    oblast[i][j].rasnost_coord[i1][j1] = 0;
                }
            }
            for (int i1 = 0;i1 < n * m;i1++) {
                oblast[i][j].kovariaz_matrix[i1].resize(n * m);
                for (int j1 = 0;j1 < n * m;j1++) {
                    oblast[i][j].kovariaz_matrix[i1][j1] = 0;
                }
            }
        }
    }

    // считываем образы, заполняем матрицы координат и выводим образы в консоль
    for (int step = 0; step < clauses_amount; step++) {
        for (int obr = 0;obr < image_amount;obr++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    oblast[step][obr].coord[i][j] = loader.examples[step*clauses_amount + obr][i][j];
                }
            }
        }
    }

    // вычисляем ядра классов (математическое ожидание)
    for (int step = 0;step < clauses_amount;step++) {
        cout << "Core " << step + 1 << ":\n";
        for (int i = 0;i < n;i++) {
            for (int j = 0;j < m;j++) {
                for (int obr = 0;obr < image_amount;obr++) {
                    oblast[step][0].core[i][j] += oblast[step][obr].coord[i][j];
                }
                oblast[step][0].core[i][j] /= (n * m);
                cout << oblast[step][0].core[i][j] << "\t";
            }
            cout << endl;
        }
        print_line();
    }

    // вычисляем матрицу ковариации
    for (int step = 0;step < clauses_amount;step++) {
        for (int i = 0;i < n * m;i++) {
            for (int j = 0;j < n * m;j++) {
                for (int obr = 0;obr < image_amount;obr++) {
                    oblast[step][0].kovariaz_matrix[i][j] += (oblast[step][obr].coord[i / n][i % n] - oblast[step][0].core[i / n][i % n]) * (oblast[step][0].coord[j / m][j % m] - oblast[step][0].core[j / m][j % m]);
                }
                oblast[step][0].kovariaz_matrix[i][j] /= (n * m - 1);
            }
        }
        // преобразуем S в (S+E)^-1
        E_M(oblast[step][0].kovariaz_matrix);
    }


    //считываем новые образы и вычисляем (x - y)^T
    for (int st = 0; st < task_image_amount; st++) {
        for (int i = 0;i < n;i++) {
            for (int j = 0;j < m;j++) {
                for (int step = 0;step < clauses_amount;step++) {
                    oblast[step][0].rasnost_coord[i][j] = loader.tasks[st][i][j] - oblast[step][0].core[i][j];
                }
            }
        }
    
        for (int step = 0;step < clauses_amount;step++) {
            for (int i = 0;i < oblast[step][0].kovariaz_umnog.size();i++) {
                for (int j = 0;j < oblast[step][0].kovariaz_umnog[i].size();j++) {
                    oblast[step][0].kovariaz_umnog[i][j] = 0;
                }
            }
        }

        //вычисляем (x - y)^T * (S+E)^-1
        for (int i = 0;i < n * m;i++) {
            for (int j = 0;j < n * m;j++) {
                for (int step = 0;step < clauses_amount;step++) {
                    oblast[step][0].kovariaz_umnog[i / m][i % m] += (oblast[step][0].rasnost_coord[j / m][j % m] * oblast[step][0].kovariaz_matrix[i][j]);
                }
            }
        }

        //вычисляем (x - y)^T * (S+E)^-1 * (x - y)
        for (int i = 0;i < n;i++) {
            for (int j = 0;j < m;j++) {
                for (int step = 0;step < clauses_amount;step++) {
                    oblast[step][0].final += (oblast[step][0].kovariaz_umnog[i][j] * oblast[step][0].rasnost_coord[i][j]);
                }
            }
        }

        vector<pair<float, int>> best;
        for (int step = 0;step < clauses_amount;step++) {
            oblast[step][0].final = sqrt(oblast[step][0].final);
            best.push_back(make_pair(oblast[step][0].final, step));
        }
        sort(best.begin(), best.end());

        loader.print_tasks(st, loader.classes[best[0].second * 4]);
        for (int step = 0;step < clauses_amount;step++) {
            cout << "Distance " << loader.classes[step * 4] << " : ";
            cout << oblast[step][0].final << endl;
        }
    }




    return 0;
}