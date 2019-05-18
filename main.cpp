#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>
#include <limits>
#include <map>

using namespace std;
using byte = unsigned char;
using bits_in_byte = bitset<size_t(8)>;
constexpr std::size_t BITS_PER_BYTE = std::numeric_limits<byte>::digits;
typedef map<string, int>  Map; // упрощение, чтобы не писать полностью объявление словаря


class archive
{
	string buffer; // буфер для хранения

	ofstream output; // выходной файл
	ifstream input; //входной файл
public:
	void compress(char const**, int); //сжатие
	string compress1(string);// сжатие 77
	int len(int); //???
	string to_binary_string(unsigned long int, int); // перевод в бинарное значение
	string write_bits(string);
	void decompress(string); //деархивация
	int bits_to_int(string); //биты в инт. Используем при деархивации
	string get_bits(int); // получаем биты. Используем при деархивации
	void write_decompress(string&); //??
	void cache_in();//получаение размера файла
	~archive();
	archive() {};
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

	//
	for (; i < size; i++)
	{
		result += (int(bits[i]) - int('0')) * int(pow(2, size - i - 1));
	}
	return result;
}
//получаем биты. Используем при деархивации
string archive::get_bits(int lenght)
{
	// if (buffer.empty())
	//     return "";
	string result = buffer.substr(0, lenght);
	buffer.erase(0, lenght);
	return result;
}
//получаение размера файла
void archive::cache_in()
{
	char c;
	buffer = "";
	while (!input.eof())
	{
		input.get(c);
		buffer += bits_in_byte(byte(c)).to_string();
	}
	cout << "caching completed ..." << endl;
	cout << "starting size: " << buffer.size() << endl;
}
void archive::Decompress() {
}
archive CreateArchive(const int& argc, char* argv[]) {
	if (argc == 3) {
		string output = argv[1];
		output += ".lzw";
		archive Archive(argv[2], output);
		return Archive;
	}
	else if (argc == 2) {
		string input = argv[1];
		for (int i = 0; i < 5; i++) {
			input.pop_back();
		}
		archive Archive(input, argv[1]);
		return Archive;
	}
	else {
		throw runtime_error("Wrong input");
	}
}
map<int, string> InvertMap(const map<string, int>& x) {
	map<int, string> result;
	for (const auto& item : x) {
		result[item.second] = item.first;
	}
	return result;
}//архивация
void archive::compress(char const** argv, int argc)
{
	string bitstring = "";
	//*********************************
	//открываем файл и записываем его содержимое в виде битов в строку для хранения
	for (int i = 2; i < argc - 1; i++)
	{
		cout << "Compressing file " << argv[i] << "... ";
		input.open(argv[i], ios::binary);
		if (!input.is_open()) {
			cout << "Error!" << endl;
			continue;
		}
		bitstring += write_bits(argv[i]);
		input.close();
		cout << "Done!" << endl;
	}
	bitstring = compress1(bitstring); // архивируем содержимое
	bitstring.erase(0, 1); //перепроверяем-с bitstring на чистоту
	
	//Завершаем незавершенный байт, если таковой имеется
	while (bitstring.size() % BITS_PER_BYTE)
	{
		cout << "0";
		bitstring += '0';
	}
	cout << endl;

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
	bitstring += to_binary_string(buff.size(), 64); // переводим биты в байты
	bitstring.append(buff); // добавляем содержимое файла в байтах в последовательность байтов файла
	return bitstring;
}
//процесс архивации LZ78
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
	archive rar1; // создаем переменную рар

	// получаем параметр
	string parameter(argv[1]);

	//выводим параметр
	cout << parameter << endl;

	if (parameter == "--decompress") // деархивация
	{
		string path_to_archive(argv[2]);
		rar1.decompress(path_to_archive);
		cout << argc - 2 << "files written." << endl;
	}
	if (parameter == "--compress") // архивация
	{
		rar1.compress(argv, argc);
		cout << "Result written to " << argv[argc - 1] << endl;
	}
	return 0;
}
