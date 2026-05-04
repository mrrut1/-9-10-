/*Вариант 7.
Определить комбинированный (структурный) тип для представления информации
по океаническим впадинам, состоящей из названия впадины, ее глубины, океане
местонахождения, типе впадины (разломная, хребет, желоб и др.). Ввести
информацию по 20 впадинам. Вывести минимальное и максимальное значение по
всем 20 впадинам. Затем вывести информацию по всем желобным впадинам,
отсортированную по убыванию глубины впадины (рационально переставлять все
поля структуры разом). Вывести сведения по океанам местонахождения 3-х
глубочайших впадин. Реализовать функцию изменения данных впадины по ее
глубине, а не по названию. В отдельный массив поместить все впадины одного
океана (океан вводить с клавиатуры). Реализовать вывод отфильтрованных данных
в виде оберточной функции.*/

/*Задание 1. Считать из текстового файла данные числового поля для вашей.
Например, исходный файл имеет 2 столбца: ФИО и пол (0 – мужской, 1 -
женский). Из файла считываются данные и если фамилия совпадает с
очередным элементом массива структур, то в поле pol записывается
соответствующее значение из файла.
Ivanov 0
Ivanova 1
Petrov 0
Sidorova 1
Makarkin 0
Задание 2. Сделать чтение / запись своей структуры в бинарный файл.
Оформить в виде подпрограмм.*/
#include <iostream>
#include <string>
#include <fstream> 
#include <cstring>  

using namespace std;  

// 9 лаба 7 вар
enum TrenchType {
    RIFT,
    RIDGE,
    TROUGH,
    OTHER
};

struct Ocean {
    string name;
    double area;
};

struct Trench {
    string name;
    double depth;
    Ocean ocean;
    TrenchType type;
};

void readDepthFromTextFile(Trench trenches[], int size, const char* filename);
void writeTrenchesToBinaryFile(const Trench trenches[], int size, const char* filename);
bool readTrenchesFromBinaryFile(Trench trenches[], int& size, const char* filename, int maxSize);

void swapTrenches(Trench& a, Trench& b) {
    Trench temp = a;
    a = b;
    b = temp;
}

string typeToString(TrenchType type) {
    switch (type) {
    case RIFT: return "Разломная";
    case RIDGE: return "Хребет";
    case TROUGH: return "Желоб";
    default: return "Другой тип";
    }
}

void printTrench(const Trench& t) {
    cout << "Название: " << t.name
        << ", Глубина: " << t.depth << " м"
        << ", Океан: " << t.ocean.name
        << ", Площадь океана: " << t.ocean.area << " млн км²"
        << ", Тип: " << typeToString(t.type) << endl;
}

void findMinMaxDepth(const Trench trenches[], int size, double& minDepth, double& maxDepth) {
    minDepth = trenches[0].depth;
    maxDepth = trenches[0].depth;

    for (int i = 1; i < size; i++) {
        if (trenches[i].depth < minDepth) {
            minDepth = trenches[i].depth;
        }
        if (trenches[i].depth > maxDepth) {
            maxDepth = trenches[i].depth;
        }
    }
}

void bubbleSortByDepth(Trench trenches[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (trenches[j].depth < trenches[j + 1].depth) {
                swapTrenches(trenches[j], trenches[j + 1]);
            }
        }
    }
}

bool editTrenchByDepth(Trench trenches[], int size, double targetDepth) {
    for (int i = 0; i < size; i++) {
        if (trenches[i].depth == targetDepth) {
            cout << "\nРедактирование впадины: " << trenches[i].name << endl;
            cout << "Текущая глубина: " << trenches[i].depth << " м" << endl;

            cout << "Введите новое название: ";
            cin.ignore();
            getline(cin, trenches[i].name);

            cout << "Введите новую глубину: ";
            cin >> trenches[i].depth;

            cout << "Введите название океана: ";
            cin.ignore();
            getline(cin, trenches[i].ocean.name);

            cout << "Введите площадь океана (млн км²): ";
            cin >> trenches[i].ocean.area;

            int typeChoice;
            cout << "Выберите тип впадины (0-Разломная, 1-Хребет, 2-Желоб, 3-Другой): ";
            cin >> typeChoice;
            trenches[i].type = static_cast<TrenchType>(typeChoice);

            return true;
        }
    }
    return false;
}

void filterByOcean(const Trench trenches[], int size, const string& oceanName, Trench result[], int& resultSize) {
    resultSize = 0;
    for (int i = 0; i < size; i++) {
        if (trenches[i].ocean.name == oceanName) {
            result[resultSize++] = trenches[i];
        }
    }
}

void displayFilteredData(const Trench trenches[], int size) {
    if (size == 0) {
        cout << "Нет данных для отображения." << endl;
        return;
    }

    cout << "\n Отфильтрованные данные " << endl;
    for (int i = 0; i < size; i++) {
        printTrench(trenches[i]);
    }
}

void printOceansOfDeepest(const Trench trenches[], int size) {
    cout << "\n Океаны местонахождения 3-х глубочайших впадин " << endl;
    int count = (size < 3) ? size : 3;
    for (int i = 0; i < count; i++) {
        cout << i + 1 << ". " << trenches[i].name
            << " (глубина: " << trenches[i].depth << " м)"
            << " - Океан: " << trenches[i].ocean.name << endl;
    }
}

// Задание 1
void readDepthFromTextFile(Trench trenches[], int size, const char* filename) {
    ifstream fin;
    fin.open(filename);

    if (!fin.is_open()) {
        cout << "Ошибка: не удалось открыть файл " << filename << endl;
        return;
    }

    char name[80];
    double newDepth;
    bool found;

    cout << "\nЧтение данных из текстового файла" << endl;

    while (fin >> name >> newDepth) {
        found = false;
        for (int i = 0; i < size; i++) {
            if (strcmp(trenches[i].name.c_str(), name) == 0) {
                cout << "Обновлена глубина впадины \"" << trenches[i].name
                    << "\": " << trenches[i].depth << " м -> " << newDepth << " м" << endl;
                trenches[i].depth = newDepth;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Впадина \"" << name << "\" не найдена в массиве" << endl;
        }
    }

    fin.close();
}

// Задание 2
void writeTrenchesToBinaryFile(const Trench trenches[], int size, const char* filename) {
    ofstream fout(filename, ios::binary | ios::out);

    if (!fout.is_open()) {
        cout << "Ошибка: не удалось открыть файл для записи" << endl;
        return;
    }

    fout.write((char*)&size, sizeof(size));
    fout.write((char*)trenches, sizeof(Trench) * size);

    fout.close();
    cout << "Данные записаны в бинарный файл: " << filename << endl;
}

bool readTrenchesFromBinaryFile(Trench trenches[], int& size, const char* filename, int maxSize) {
    ifstream fin(filename, ios::binary | ios::in);

    if (!fin.is_open()) {
        cout << "Ошибка: не удалось открыть файл для чтения" << endl;
        return false;
    }

    int fileSize;
    fin.read((char*)&fileSize, sizeof(fileSize));

    if (fileSize > maxSize) {
        cout << "Ошибка: размер файла превышает максимум" << endl;
        fin.close();
        return false;
    }

    fin.read((char*)trenches, sizeof(Trench) * fileSize);
    size = fileSize;

    fin.close();
    cout << "Данные прочитаны из бинарного файла: " << filename << endl;
    return true;
}

int main() {
    setlocale(LC_ALL, "Russian");

    const int SIZE = 20;
    Trench trenches[SIZE] = {
        {"Марианская", 11034, {"Тихий", 165.2}, TROUGH},
        {"Тонга", 10882, {"Тихий", 165.2}, TROUGH},
        {"Филиппинская", 10540, {"Тихий", 165.2}, TROUGH},
        {"Кермадек", 10047, {"Тихий", 165.2}, TROUGH},
        {"Идзу-Бонин", 9810, {"Тихий", 165.2}, TROUGH},
        {"Пуэрто-Рико", 8800, {"Атлантический", 106.5}, TROUGH},
        {"Южно-Сандвичев", 8428, {"Атлантический", 106.5}, TROUGH},
        {"Романш", 7856, {"Атлантический", 106.5}, RIFT},
        {"Яванская", 7450, {"Индийский", 70.6}, TROUGH},
        {"Зондская", 7729, {"Индийский", 70.6}, TROUGH},
        {"Алеутская", 7679, {"Тихий", 165.2}, RIFT},
        {"Перуанско-Чилийская", 8065, {"Тихий", 165.2}, TROUGH},
        {"Курило-Камчатская", 10542, {"Тихий", 165.2}, TROUGH},
        {"Японская", 8412, {"Тихий", 165.2}, RIDGE},
        {"Литке", 6200, {"Северный Ледовитый", 14.1}, RIFT},
        {"Нансена", 5500, {"Северный Ледовитый", 14.1}, RIDGE},
        {"Макарова", 5400, {"Северный Ледовитый", 14.1}, OTHER},
        {"Амундсена", 5800, {"Северный Ледовитый", 14.1}, RIFT},
        {"Мозамбикская", 4500, {"Индийский", 70.6}, RIFT},
        {"Австрало-Антарктическая", 4800, {"Индийский", 70.6}, RIDGE}
    };

    ofstream testFile("depths.txt");
    if (testFile.is_open()) {
        testFile << "Марианская 11000\n";
        testFile << "Тонга 10850\n";
        testFile << "Филиппинская 10500\n";
        testFile.close();
        cout << "Создан тестовый файл depths.txt" << endl;
    }

    readDepthFromTextFile(trenches, SIZE, "depths.txt");
    writeTrenchesToBinaryFile(trenches, SIZE, "trenches.bin");

    Trench loadedTrenches[SIZE];
    int loadedSize = 0;
    readTrenchesFromBinaryFile(loadedTrenches, loadedSize, "trenches.bin", SIZE);

    double minDepth, maxDepth;
    findMinMaxDepth(trenches, SIZE, minDepth, maxDepth);
    cout << "Минимальная глубина среди всех впадин: " << minDepth << " м" << endl;
    cout << "Максимальная глубина среди всех впадин: " << maxDepth << " м" << endl;

    cout << "\n Все желобные впадины" << endl;

    Trench troughTrenches[SIZE];
    int troughCount = 0;

    for (int i = 0; i < SIZE; i++) {
        if (trenches[i].type == TROUGH) {
            troughTrenches[troughCount++] = trenches[i];
        }
    }

    bubbleSortByDepth(troughTrenches, troughCount);

    for (int i = 0; i < troughCount; i++) {
        printTrench(troughTrenches[i]);
    }

    Trench allTrenches[SIZE];
    for (int i = 0; i < SIZE; i++) {
        allTrenches[i] = trenches[i];
    }
    bubbleSortByDepth(allTrenches, SIZE);
    printOceansOfDeepest(allTrenches, SIZE);

    cout << "\nИзменение данных впадины по глубине" << endl;
    double searchDepth = 10882;
    if (editTrenchByDepth(trenches, SIZE, searchDepth)) {
        cout << "Данные успешно обновлены!" << endl;
    }
    else {
        cout << "Впадина с глубиной " << searchDepth << " не найдена!" << endl;
    }

    string targetOcean;
    cout << "\nВведите название океана для фильтрации (Тихий/Атлантический/Индийский/Северный Ледовитый): ";
    cin.ignore();
    getline(cin, targetOcean);

    Trench filteredTrenches[SIZE];
    int filteredCount = 0;
    filterByOcean(trenches, SIZE, targetOcean, filteredTrenches, filteredCount);

    cout << "\nВпадины в океане \"" << targetOcean << "\":" << endl;
    displayFilteredData(filteredTrenches, filteredCount);

    return 0;
}
