#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

class archive {
private:
	int size;
	int counter;
	string debug;
	string input;
	string output;
	map<string, int> dict_original;
public:
	archive(const string& input, const string& output);
	void BuildDict();
	void Compress();
};

archive::archive(const string& input, const string& output) {
	this->input = input;
	this->output = output;
	counter = 0;
	size = 0;
}
void archive::BuildDict() {
	//cout << "\n\nFilling the dictionary...\n";

	//Создал входящий поток
	ifstream stream(input, ios::binary);

	//Создал переменную строку и проинициализировал 1 символом
	//т.к. в цикле while ниже я первым делом вызываю pop_back() у строки
	string temp = ".";

	//просто символ, который будет таскать символы из файла
	char x;

	while (stream) {
		//очищаю строку
		temp.pop_back();

		//получаю символ из файла и пушу его в строку
		stream.get(x);
		temp.push_back(x);

		//проверяю словарь на наличие совпадений
		//если есть, то следующая итерация
		if (dict_original.find(temp) != dict_original.end()) continue;

		//иначе создаю пару в словаре и вывожу на экран(при отладке)
		dict_original[temp] = ++counter;
		//cout << temp << " " << dict_original[temp] << endl;
	}

	//закрываю файл
	stream.close();

	//запоминаю размер "оригинального" словаря(односимвольных уникальных фраз)
	size = dict_original.size();

	//cout << "Filled the dictionary!\n" << endl;
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
			out.write((char*)& temp_t, sizeof(temp_t));
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

int main() {
	string input = "C:\\a\\w.txt";
	archive Archive(input, input + ".lzw");
	Archive.BuildDict();
	Archive.Compress();
	system("pause");
	return 0;
}