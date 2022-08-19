#include<iostream>
#include<vector>
#include <tuple>
#include <utility>
#include <optional>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

// Task 1 ===============================================

struct Person
{
	string surname;
	string name;
	optional<string> patronymic;

	friend bool operator== (const Person& p1, const Person& p2);
	friend bool operator< (const Person & p1, const Person & p2);
};

bool operator== (const Person& p1, const Person& p2)
{
	return tie(p1.surname, p1.name, p1.patronymic) == tie(p2.surname, p2.name, p2.patronymic);
}

bool operator< (const Person & p1, const Person & p2)
{
	return tie(p1.surname, p1.name, p1.patronymic) < tie(p2.surname, p2.name, p2.patronymic);
}

ostream& operator<< (ostream& out, Person& pers)
{
	out << pers.surname << " " << pers.name << (pers.patronymic.has_value() ? " " : "") << pers.patronymic.value_or("") << " ";
	return out;
}

// Task 2 ===============================================

struct PhoneNumber
{
	int countryCode;
	int cityCode;
	string numberMain;
	optional<int> numberAdd;

	friend bool operator== (const PhoneNumber& p1, const PhoneNumber& p2);
	friend bool operator< (const PhoneNumber& p1, const PhoneNumber& p2);
};

bool operator== (const PhoneNumber& num1, const PhoneNumber& num2)
{
	return tie(num1.countryCode, num1.cityCode, num1.numberMain, num1.numberMain) == tie(num2.countryCode, num2.cityCode, num2.numberMain, num2.numberMain);
}

bool operator< (const PhoneNumber& num1, const PhoneNumber& num2)
{
	return tie(num1.countryCode, num1.cityCode, num1.numberMain, num1.numberAdd) < tie(num2.countryCode, num2.cityCode, num2.numberMain, num2.numberAdd);
}

ostream& operator<< (ostream& out, PhoneNumber& num)
{
	out << "+" << num.countryCode << "(" << num.cityCode << ")" << num.numberMain << " ";
	if (num.numberAdd.has_value())
		out << num.numberAdd.value_or(-1);
	return out;
}

// Task 3 ===============================================

class PhoneBook
{
private:
	vector<pair<Person, PhoneNumber>> data;

public:
	PhoneBook(ifstream& file)
	{
		string buf;
		
		while (!file.eof())
		{
			data.push_back(make_pair(Person(), PhoneNumber()));
			
			getline(file, data.back().first.surname, ' ');
			getline(file, data.back().first.name, ' ');
			if(file.peek() != '+')
			{
				getline(file, buf, ' ');
				data.back().first.patronymic = buf;
				file.get();
			}
			else
			{
				data.back().first.patronymic = nullopt;
				file.get();
			}
			
			getline(file, buf, '(');
			data.back().second.countryCode = stoi(buf);
			getline(file, buf, ')');
			data.back().second.cityCode = stoi(buf);
			file >> data.back().second.numberMain;
			if (file.peek() != '\n' && !file.eof())
			{
				getline(file, buf, '\n');
				data.back().second.numberAdd = stoi(buf);
			}
			else
			{
				data.back().second.numberAdd = nullopt;
				file.get();
			}
		}
	}

	void SortByName()
	{
		sort(data.begin(), data.end(), [](pair<Person, PhoneNumber> pair1, pair<Person, PhoneNumber> pair2) {

			return pair1.first < pair2.first;

		});
	}

	void SortByPhone()
	{
		sort(data.begin(), data.end(), [](pair<Person, PhoneNumber> pair1, pair<Person, PhoneNumber> pair2) {

			return pair1.second < pair2.second;

		});
	}

	tuple<string, PhoneNumber> GetPhoneNumber(string surname)
	{
		int counter(0);
		PhoneNumber search;
		for (auto& it : data)
		{
			if (it.first.surname == surname)
			{
				counter++;
				if (counter > 1)
					return make_tuple("found more than 1", search);
				search = it.second;
			}
		}

		if (counter == 1)
			return make_tuple("", search);

		return make_tuple("not found", search);
	}
	
	void ChangePhoneNumber (Person person, PhoneNumber phoneNumber)
	{
		for (auto& it : data)
		{
			if (it.first == person)
			{
				it.second = phoneNumber;
			}
		}
	}

	friend ostream& operator<< (ostream& out, PhoneBook& phBook);
};

ostream& operator<< (ostream& out, PhoneBook& phBook)
{
	for (auto& it : phBook.data)
	{
		out << it.first << it.second << endl;
	}

	return out;
}



int main()
{
	setlocale(LC_ALL, "Russian");
	ifstream file("PhoneBook.txt");
	
	if (file.is_open()) { cout << "File was opened " << endl; }
	else { return 1; }
	
	PhoneBook book(file);

	cout << book;
	cout << "------SortByPhone-------" << endl;
	book.SortByPhone();
	cout << book;
	cout << "------SortByName--------" << endl;
	book.SortByName();
	cout << book;
	cout << "-----GetPhoneNumber-----" << endl;
		auto print_phone_number = [&book](const string& surname) {
		cout << surname << "\t";
		auto answer = book.GetPhoneNumber(surname);
		if (get<0>(answer).empty())
			cout << get<1>(answer);
		else
			cout << get<0>(answer);
		cout << endl;
	};
	print_phone_number("Ivanov");
	print_phone_number("Дубинин");
	cout << "----ChangePhoneNumber----" << endl;
	book.ChangePhoneNumber(Person{ "Kotov", "Vasilii", "Eliseevich" },
		PhoneNumber{ 7, 123, "15344458", nullopt });
	book.ChangePhoneNumber(Person{ "Титова", "Наталья", nullopt },
		PhoneNumber{ 16, 465, "9155448", 13 });
	cout << book;

}