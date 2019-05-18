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
void archive::Compress() {
	cout << "\n\nCompressing file " << input << "...\n";

	//Создаю два файловых потока
	ifstream in(input, ios::binary);
	ofstream out(output);

	//Так мы записываем кол-во элементов в словаре
	//(или сколько нужно итераций цикла для алгоритма декомпресса)
	out.write((char*)& size, sizeof(size));
	//out << size;

	//После записи кол-ва элементов, записываем все элементы словаря
	//(так, как используем std::map, а это r&b tree, то они будут по порядку)
	//Таким образом, при декомпрессинге, мы сможем заполнить заново "односимвольный" словарь
	// просто прочитав ту часть файла, которую запишем
	for (const auto& item : dict_original) {
		out.write((char*)& item.first, sizeof(item.first));
		//out << item.first;
	}

	//Создал второй словарь, который уже сможет хранить многосимвольные фразы
	map<string, int> dict = dict_original;

	//Создал переменную строку(буфер) и указатель на чары(ниже в алгоритме пояснено)
	string temp_s;
	char* temp;

	//просто символ, который будет таскать символы из файла
	char temp_c;

	//Теперь алгоритм компресса самого файла
	while (in) {
		//Получаю из файла символ
		in.get(temp_c);

		//Если есть совпадение буфер+символ в словаре, прибавляю его в буфер и к следующей итерации
		if (dict.find(temp_s + to_string(temp_c)) != dict.end()) {
			temp_s += to_string(temp_c);
		}
		//Если же нет совпадения, то
		else {
			//cout << "\n\nbegin\n\n" << temp_s << "\n\n" << temp_s.c_str() << "\n\nend\n\n";
			//Записываю в файл тот буфер, который имеется на данный момент
			int temp_t = dict[temp_s];
			out.write((char*)&temp_t, sizeof(temp_t));
			//out << temp_t;

			//Делаю новую запись в словаре с данной строкой
			dict[temp_s + to_string(temp_c)] = ++counter;

			//Буфер очищается и приравнивается к текущему значению последнего символа
			temp_s = to_string(temp_c);
		}
	}

	//закрываю файлы
	in.close();
	out.close();

	//Обнуляю счетчик для возможного декомпрессинга
	counter = 0;
	
	cout << "Compressed file " << input << "!" << endl;
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
}
void Process(archive& Archive, const string& command) {
	if (command == "--compress") {
		Archive.BuildDict();
		Archive.Compress();
	}
	else if (command == "--decompress") {
		Archive.Decompress();
	}
}

int main(int argc, char* argv[]) {
	//Создаем архив
	archive Archive = CreateArchive(argc, argv);
	//Выполняем или компрессию, или декомпрессию, в зависимости от указанной комманды
	Process(Archive, argv[0]);

	////Кусок кода для работы компрессора изнутри программы
	//string input = "C:\\a\\w.txt";
	//archive Archive(input, input + ".lzw");
	//Archive.BuildDict();
	//Archive.Compress();

	////Кусок кода для работы декомпрессора изнутри программы
	//string output = "C:\\a\\w.txt.lzw";
	//string input = output;
	//for (int i = 0; i < 5; i++) {
	//	input.pop_back();
	//}
	//archive Archive(input, output);

	system("pause");
	return 0;
}
