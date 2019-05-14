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

	//������ �������� �����
	ifstream stream(input, ios::binary);

	//������ ���������� ������ � ������������������ 1 ��������
	//�.�. � ����� while ���� � ������ ����� ������� pop_back() � ������
	string temp = ".";

	//������ ������, ������� ����� ������� ������� �� �����
	char x;

	while (stream) {
		//������ ������
		temp.pop_back();

		//������� ������ �� ����� � ���� ��� � ������
		stream.get(x);
		temp.push_back(x);

		//�������� ������� �� ������� ����������
		//���� ����, �� ��������� ��������
		if (dict_original.find(temp) != dict_original.end()) continue;

		//����� ������ ���� � ������� � ������ �� �����(��� �������)
		dict_original[temp] = ++counter;
		//cout << temp << " " << dict_original[temp] << endl;
	}

	//�������� ����
	stream.close();

	//��������� ������ "�������������" �������(�������������� ���������� ����)
	size = dict_original.size();

	//cout << "Filled the dictionary!\n" << endl;
}
void archive::Compress() {
	cout << "\n\nCompressing file " << input << "...\n";

	//������ ��� �������� ������
	ifstream in(input, ios::binary);
	ofstream out(output);

	//��� �� ���������� ���-�� ��������� � �������
	//(��� ������� ����� �������� ����� ��� ��������� �����������)
	out.write((char*)& size, sizeof(size));
	//out << size;

	//����� ������ ���-�� ���������, ���������� ��� �������� �������
	//(���, ��� ���������� std::map, � ��� r&b tree, �� ��� ����� �� �������)
	//����� �������, ��� ��������������, �� ������ ��������� ������ "��������������" �������
	// ������ �������� �� ����� �����, ������� �������
	for (const auto& item : dict_original) {
		out.write((char*)& item.first, sizeof(item.first));
		//out << item.first;
	}

	//������ ������ �������, ������� ��� ������ ������� ��������������� �����
	map<string, int> dict = dict_original;

	//������ ���������� ������(�����) � ��������� �� ����(���� � ��������� ��������)
	string temp_s;
	char* temp;

	//������ ������, ������� ����� ������� ������� �� �����
	char temp_c;

	//������ �������� ��������� ������ �����
	while (in) {
		//������� �� ����� ������
		in.get(temp_c);

		//���� ���� ���������� �����+������ � �������, ��������� ��� � ����� � � ��������� ��������
		if (dict.find(temp_s + to_string(temp_c)) != dict.end()) {
			temp_s += to_string(temp_c);
		}
		//���� �� ��� ����������, ��
		else {
			//cout << "\n\nbegin\n\n" << temp_s << "\n\n" << temp_s.c_str() << "\n\nend\n\n";
			//��������� � ���� ��� �����, ������� ������� �� ������ ������
			int temp_t = dict[temp_s];
			out.write((char*)& temp_t, sizeof(temp_t));
			//out << temp_t;

			//����� ����� ������ � ������� � ������ �������
			dict[temp_s + to_string(temp_c)] = ++counter;

			//����� ��������� � �������������� � �������� �������� ���������� �������
			temp_s = to_string(temp_c);
		}
	}

	//�������� �����
	in.close();
	out.close();

	//������� ������� ��� ���������� ��������������
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