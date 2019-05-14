#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

class archive {
private:
	static int counter;
	string debug;
	string input;
	string output;
	map<string, int> dict_original;
public:
	archive();
	archive(const string &input, const string &output);
	void BuildDict();
	void Compress();
};

archive::archive() {}
archive::archive(const string &input, const string &output) {
	this->input = input;
	this->output = output;
	counter = 0;
}
void archive::BuildDict() {
	cout << "\n\n========= Filling the first dictionary =========\n" << endl;
	ifstream stream("C:\\a\\w.txt", ios::binary);

	for (const auto& x : debug) {
		string temp;
		temp.push_back(x);
		if (dict_original.find(temp) != dict_original.end()) continue;
		dict_original[temp] = ++counter;
		cout << temp << " " << dict_original[temp] << endl;
	}
}
void archive::Compress() {

}


int main() {
	map<string, int> dict = dict_original;
	string temp_s;
	char temp_c;
	string encoded = to_string(dict_original.size());

	cout << "\n\n========= Encoding the file =========\n" << endl;
	for (const auto &item : dict_original) {
		cout << item.first << " : " << item.second << endl;
		encoded += item.first;
	}
	for (rsize_t i = 0; i < file.length(); i++) {
		temp_c = file[i];
		if (dict.find(temp_s + to_string(temp_c)) != dict.end()) {
			temp_s += to_string(temp_c);
		}
		else {
			encoded += to_string(dict[temp_s]);
			encoded += ',';
			dict[temp_s + to_string(temp_c)] = ++counter;
			temp_s = to_string(temp_c);
		}
	}
	cout << "Your code: " << encoded << endl;
	cout << "File size = " << file.length() << "; Code size = " << encoded.length() - file.length() << endl;

	system("pause");
	return 0;
}
void test() {
	string file = "eejhbasvkjndfvkjncvm,ncv,nm,vmcnvb,mnzvcv;sdjjrkvbcnvmxawehjbjhbskdvjbdskvjlxdjfsef";
	cout << "File: " << file << endl;
	int counter = 0;
	map<string, int> dict_original;
	cout << "\n\n========= Filling the first dictionary =========\n" << endl;
	for (const auto& x : file) {
		string temp;
		temp.push_back(x);
		if (dict_original.find(temp) != dict_original.end()) continue;
		dict_original[temp] = ++counter;
		cout << temp << " " << dict_original[temp] << endl;
	}
	map<string, int> dict = dict_original;
	string temp_s;
	char temp_c;
	string encoded = to_string(dict_original.size());

	cout << "\n\n========= Encoding the file =========\n" << endl;
	for (const auto &item : dict_original) {
		cout << item.first << " : " << item.second << endl;
		encoded += item.first;
	}
	for (rsize_t i = 0; i < file.length(); i++) {
		temp_c = file[i];
		if (dict.find(temp_s + to_string(temp_c)) != dict.end()) {
			temp_s += to_string(temp_c);
		}
		else {
			encoded += to_string(dict[temp_s]);
			encoded += ',';
			dict[temp_s + to_string(temp_c)] = ++counter;
			temp_s = to_string(temp_c);
		}
	}
	cout << "Your code: " << encoded << endl;
	cout << "File size = " << file.length() << "; Code size = " << encoded.length() - file.length() << endl;
}