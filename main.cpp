#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>
#include <algorithm>
#include <limits>
#include <unordered_map>

using namespace std;
using byte = unsigned char;
using bits_in_byte = bitset<size_t(8)>;
constexpr std::size_t BITS_PER_BYTE = std::numeric_limits<byte>::digits;
typedef unordered_map<string, int>  Map; // упрощение, чтобы не писать полностью объявление словаря


class archive
{
	string buffer; // буфер для хранения
	char *buffer1;
	long size;
	ofstream output; // выходной файл
	ifstream input; //входной файл
public:
	void compress(char const**, int); //сжатие
	string compress1(string);// сжатие 78
	int len(int);  //та же функция для вычисления длины "бинарной строки"
	string to_binary_string(unsigned long int, int); // перевод в бинарное значение
	string write_bits(string);
	void decompress(string); //деархивация
	int bits_to_int(string); //биты в инт. Используем при деархивации
	string get_bits(int); // получаем биты. Используем при деархивации
	void write_decompress(string&); //декомпрессия
	void cache_in();//получаение размера файла
	~archive();
};
archive::~archive()
{
	//закрытие файлов во избежание утечки кармы
	if (output.is_open()) output.close();
	if (input.is_open()) input.close();
}
//биты в инт. Используем при деархивации
int archive::bits_to_int(string bits)
{
	//инициализация переменных
	int result = 0, i = 0, size = bits.size();

	//смещение на первый ненулевой бит
	while (bits[i] == 0)
		i++;
	for (; i < size; i++)
	{
		result += (int(bits[i]) - int('0')) * int(pow(2, size - i - 1));
	}
	return result;
}
//получаем биты. Используем при деархивации
string archive::get_bits(int lenght)
{
	string result = buffer.substr(0, lenght);
	buffer.erase(0, lenght);
	return result;
}
//void archive::cache_in()
//{
//	char c;
//	buffer = "";
//	while (!input.eof())
//	{
//		input.get(c);
//		buffer += bits_in_byte(byte(c)).to_string();
//	}
//	/*cout << "caching completed ..." << endl;
//	cout << "starting size: " << buffer.size() << endl;*/
//}
//деархивация
void archive::decompress(string path)
{
	//считываем файлик
	input.open(path, ios::binary | ios::ate);

	input.seekg(0, input.end);
	size = input.tellg();
	input.seekg(0, input.beg);
	
	buffer1 = new char[size*8];

	int temp = 0;
	char c;
	string temp_s;
	int ii = 0;
	while (!input.eof()) {
		input.get(c);
		temp_s = bits_in_byte(byte(c)).to_string();
		int length = strlen(temp_s.c_str());
		memcpy(buffer1+ii, temp_s.c_str(), length);
		ii += length;
	}

	string buffer(buffer1);

	//инициализация переменных
	string buff = "", newItem = "";
	string result = "";
	int i = 0, k = 0;

	//Создаем первое поле в векторе стрингОв "словарик" - пустая строка
	vector<string> dictionary;
	dictionary.push_back(buff);

	//получаем первый бит
	buff = get_bits(1);

	//пушим его в "словарик"
	dictionary.push_back(buff);

	//плюсуем значение второго поля в векторе(в данном случае, первый бит) в строку "результат"
	result.append(dictionary[1]);
	cout << "Entering the loop" << endl;
	//циклик
	while (!buffer.empty())
	{
		for (int j = 0; j < pow(2, k) && !buffer.empty(); j++)
		{
			cout << "Inside" << endl;
			//Получаем определенное кол-во битов
			buff = get_bits(k + 2);
			i += k + 2;
			cout << "part 1" << endl;
			//Достаем из словарика строку
			newItem = dictionary[bits_to_int(buff.substr(0, k + 1))];
			if (buff.size() > k + 1)
				newItem += buff.substr(k + 1);
			cout << "part 2" << endl;
			//Пушим строку в словарик
			dictionary.push_back(newItem);
			//Добавляем строку к строке "результат"
			result.append(newItem);
			cout << "part 3" << endl;
		}
		k++;
	}
	//Непосредственно записи
	write_decompress(result);
}

void archive::write_decompress(string & bitstring)
{
	ofstream out;
	//имя файла
	string filename = "";

	//размер файла
	int size_of_file;
	int i = 0, limit = bitstring.size() - bitstring.size() % 8;
	char c;

	//цикл до конца файла
	while (i + 8 < limit)
	{
		c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
		i += 8;
		//Пока не "конец записанного файла", а-ля "звук", то
		while (c != '\a' && i + 8 < limit)
		{
			filename += c;
			c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
			i += 8;
		}
		cout << "Getting out file " << filename << "... Done!" <<endl;
		out.open(filename, ios::binary);

		//Сколько нужно записывать
		size_of_file = bits_to_int(bitstring.substr(i, 64));
		i += 64;

		//Собираем батик
		c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
		//Адаптируем к "сборке" ^ счетчик
		i += 8;
		size_of_file -= 8;
		//Цикл до конца файла
		while (size_of_file > 0)
		{
			out << c;
			//Записываем по байтику
			c = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
			i += 8;
			size_of_file -= 8;
		}
		//закрываем файлик
		out.close();
		filename = "";
	}
}
//архивация
void archive::compress(char const** argv, int argc)
{
	string bitstring = "";
	//*********************************
	//открываем файл и записываем его содержимое в виде битов в строку для хранения
	for (int i = 2; i < argc - 1; i++)
	{
		cout << "Compressing file " << argv[i] << "... Done!" << endl;
		input.open(argv[i], ios::binary);
		bitstring += write_bits(argv[i]);
		input.close();
	}
	bitstring = compress1(bitstring); // архивируем содержимое
	bitstring.erase(0, 1); //перепроверяем-с bitstring на чистоту
	//Завершаем незавершенный байт, если таковой имеется
	while (bitstring.size() % BITS_PER_BYTE)
	{
		bitstring += '0';
	}
	//записывам результат в новый файл
	output.open(argv[argc - 1], ios::binary);
	for (size_t i = 0; i < bitstring.size(); i += BITS_PER_BYTE)
	{
		byte b = bits_in_byte(bitstring.substr(i, BITS_PER_BYTE)).to_ulong();
		output << b;
	}
}
//получаем биты для файла и его имени
string archive::write_bits(string filename)
{
	string buff = ""; // для хранения содержимого
	string bitstring = ""; // для хранения имени файла, а после и всей последовательности байтов в файле
	char c;
	for (size_t i = 0; i < filename.size(); i++) //проходимся по размеру имени файла
	{
		bitstring += bits_in_byte(byte(filename.at(i))).to_string(); //переводим биты в байты
	}
	bitstring += bits_in_byte(byte('\a')).to_string();//добавляем после названия файла битовое значение ЗВУКОВОГО СИГНАЛА для того, чтобы отделить название файла и его содержимое. При архивации и деархивации будет звуковой сигнал показывающий что файл успешно открыт
	while (input.get(c)) {
		buff += bits_in_byte(byte(c)).to_string(); //переводим биты в байты
	}
	//cout << "size of file (without name): " << buff.size() << endl; // выводим размер файла (без имени)
	bitstring += to_binary_string(buff.size(), 64); // переводим биты в байты
	bitstring.append(buff); // добавляем содержимое файла в байтах в последовательность байтов файла
	return bitstring;
}
//процесс архивации 78
string archive::compress1(string bitstring)
{

	string buff = "";
	string result = "";
	Map dictionary = {};// словарь для хранения фраза, позиция в "словарике"
	dictionary.insert(pair<string, int>("", 0)); //пушим пустую строку ключом 0
	//ну и цикл, собственно, компрессирования
	for (int i = 0; i < bitstring.size(); i++) // идем по строке
	{
		if (dictionary.find(buff + bitstring.at(i)) != dictionary.end()) // если есть совпадение "временная строка" + "текущий символ" в словаре, то
		{
			buff += bitstring.at(i); // добавляем в временную строку комбинацию
		}
		else
		{
			//Добавляем к строке "результат" 
			result += to_binary_string(dictionary[buff], len(dictionary.size()));
			result += bitstring.at(i);
			dictionary[buff + bitstring.at(i)] = dictionary.size();
			buff = "";
		}
	}
	if (buff != "")
		result += to_binary_string(dictionary[buff], len(dictionary.size()));
	return result;
}
//вспомогательная функция, достает нам необходимую длину строки для to_binary_string
int archive::len(int number)
{
	return ceil(log2(number));
}
//Переводим тот ключик из словаря, и делаем из него бинарный код, запихиваем его в строку, возвращаем строку
string archive::to_binary_string(unsigned long int n, int lenght)
{
	string result = "", buff;
	//перевод числа в двоичную систему и запись ее в строку
	do
	{
		//каждую итерацию мы записываем так, чтобы вышла инвертированная строка
		buff = result; // обнуляем буфер
		result = ('0' + (n % 2));
		result += buff;
		n = n / 2;
	} while (n > 0);
	while (result.size() < lenght)
	{
		result.insert(0, "0");
	}
	return result;
}
int main(int argc, char const* argv[])
{
	archive win; // создаем переменную win))))

	string parameter(argv[1]); // получаем параметр
	
	if (parameter == "--decompress") // деархивация
	{
		string path_to_archive(argv[2]);
		win.decompress(path_to_archive);
		cout << "1 file written" << endl;
	}
	if (parameter == "--compress") // архивация
	{
		win.compress(argv, argc);
		cout << "Result written to " << argv[argc - 1] << endl;
	}
	return 0;
}
