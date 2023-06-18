#include "NFATool.h"
#include "Utils.h"
#include "RegexParser.h"

MyVector<NFA> NFATool::automatas; // static member variable must be instantiated

void NFATool::run() {
	int key = 0;
	while (key != 1) {
		std::cout << "There are currently " << automatas.getSize() << " NFA's" << std::endl;
		std::cout << "1. Exit" << std::endl;
		std::cout << "2. Add NFA" << std::endl;
		if (automatas.getSize() > 0) {
			std::cout << "3. Select NFA at index" << std::endl;
			std::cout << "4. Remove NFA at index" << std::endl;
			std::cout << "5. Kleene star at index" << std::endl;
			std::cout << "6. Complement of NFA at index" << std::endl;
		}
		if (automatas.getSize() > 1) {
			std::cout << "7. Intersect NFA's at indices" << std::endl;
			std::cout << "8. Difference of NFA's at indices" << std::endl;
			std::cout << "9. Union of NFA's at indices" << std::endl;
			std::cout << "10. Concatenation of NFA's at indices" << std::endl;
		}
		std::cout << std::endl;

		do {
			std::cout << "Please enter key:" << std::endl;
			std::cin >> key;
		} while (!isValidKey(key));
		std::cout << std::endl;

		switch (key) {
			case 2: NFATool::add(); break;
			case 3: NFATool::select(); break;
			case 4: NFATool::remove(); break;
			case 5: NFATool::kleeneStar(); break;
			case 6: NFATool::complement(); break;
			case 7: NFATool::intersect(); break;
			case 8: NFATool::difference(); break;
			case 9: NFATool::unite(); break;
			case 10: NFATool::concatenation(); break;
		}
	}
}

bool NFATool::isValidKey(int key) {
	if (key < 1) {
		return false;
	}
	if (automatas.getSize() == 0 && key > 2) {
		return false;
	}
	if (automatas.getSize() == 1 && key > 6) {
		return false;
	}
	if (automatas.getSize() > 1 && key > 10) {
		return false;
	}
	return true;
}

void NFATool::add() {
	std::cout << "1. Add by regex" << std::endl;
	std::cout << "2. Add empty NFA" << std::endl;
	std::cout << "3. Add from file" << std::endl;
	std::cout << "4. Go back" << std::endl;

	int choice = 0;
	do {
		std::cin >> choice;
	} while (choice < 1 || choice > 4);

	if (choice == 1) {
		std::cout << "Enter regex:" << std::endl;
		MyString regex;
		std::cin >> regex;
		NFA nfa;
		try {
			nfa = NFA(regex);
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl << std::endl;
			return;
		}
		automatas.push_back(std::move(nfa));
	}
	else if (choice == 2) {
		automatas.push_back(NFA());
	}
	else if (choice == 3) {
		std::cout << "Please enter file name:" << std::endl;
		MyString filename;
		std::cin >> filename;
		NFA nfa;
		try {
			nfa.deserialize(filename);
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl << std::endl;
			return;
		}
		automatas.push_back(nfa);

	}
	std::cout << std::endl << std::endl;
}

void NFATool::select() {
	std::cout << "Enter index:" << std::endl;
	int index = 0;
	std::cin >> index;
	if (index < 0 || index >= automatas.getSize()) {
		std::cout << "Invalid index" << std::endl << std::endl;
		return;
	}
	std::cout << std::endl;
	int key = 0;

	do {
		std::cout << "Currently viewing NFA at index " << index << std::endl;
		std::cout << "1. Minimise the NFA" << std::endl;
		std::cout << "2. Determinate the NFA" << std::endl;
		std::cout << "3. Make the NFA total" << std::endl;
		std::cout << "4. Add a state to the NFA" << std::endl;
		std::cout << "5. Add a transition to the NFA" << std::endl;
		std::cout << "6. Serialize the NFA to a file" << std::endl;
		std::cout << "7. Deserialize the NFA from a file" << std::endl;
		std::cout << "8. Check if the language of the NFA is the empty language" << std::endl;
		std::cout << "9. Check if the nfa accepts a word" << std::endl;
		std::cout << "10. print the NFA" << std::endl;
		std::cout << "11. Go Back" << std::endl;

		std::cout << std::endl;
		do {
			std::cout << "please select an option:" << std::endl;
			std::cin >> key;
		} while (key < 1 || key > 11);
		std::cout << std::endl;

		switch (key) {
		case 1: automatas[index].minimise(); break;
		case 2: automatas[index].determinate(); break;
		case 3: automatas[index].makeTotal(); break;
		case 4:
		{
			bool isInitial, isFinal;
			std::cout << "is it Initial?(0-no;1-yes)" << std::endl;
			std::cin >> isInitial;
			std::cout << "is it Final?(0-no;1-yes)" << std::endl;
			std::cin >> isFinal;
			automatas[index].addState(State(isInitial, isFinal, 0));
			break;
		}
		case 5:
		{
			std::cout << "Current states: ";
			for (size_t i = 0; i < automatas[index].stateCount; i++) {
				std::cout << automatas[index].states[i].getName() << " ";
			}
			std::cout << std::endl;

			std::cout << "State index from:" << std::endl;
			size_t from;
			std::cin >> from;
			std::cout << "State index to:" << std::endl;
			size_t to;
			std::cin >> to;
			std::cout << "Transition value(letter):" << std::endl;
			char ch;
			std::cin >> ch;
			if (!(from >= automatas[index].stateCount || to >= automatas[index].stateCount || !Utils::isInAlphabet(ch))) {
				automatas[index].addTransition(Transition(from, to, ch));
			}
			automatas[index].regex = RegexParser::RegexFromNFA(automatas[index]);
			break;
		}

		case 6:
		{
			std::cout << "Enter file name:" << std::endl;
			MyString filename;
			std::cin >> filename;
			try {
				automatas[index].serialize(filename);
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			break;
		}

		case 7:
		{
			std::cout << "Enter file name:" << std::endl;
			MyString filename;
			std::cin >> filename;
			try {
				automatas[index].deserialize(filename);
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			break;
		}
		case 8:
			std::cout << (automatas[index].isEmptyLanguage() ? "Yes, it is empty" : "No, it is not empty") << std::endl;
			break;
		case 9:
		{
			std::cout << "Enter word:" << std::endl;
			MyString word;
			std::cin >> word;
			std::cout << (automatas[index].accept(word) ? "Yes, the NFA accepts the word" : "No, the NFA doesn't accept the word") << std::endl;
			break;
		}
		case 10:
			automatas[index].print();
			std::cout << std::endl;
			break;
		}
		std::cout << std::endl;
	} while (key != 11);
}

void NFATool::remove() {
	int index = 0;
	do {
		std::cout << "Enter index: (-1 to go back)" << std::endl;
		std::cin >> index;
	} while (index < -1 || index + 1 >= automatas.getSize() + 1);
	std::cout << std::endl;

	if (index == -1) {
		return;
	}

	if (index != automatas.getSize() - 1) {
		automatas[index] = std::move(automatas[automatas.getSize() - 1]);
	}
	automatas.pop_back();
}

void NFATool::kleeneStar() {
	int index = 0;
	do {
		std::cout << "Enter index: (-1 to go back)" << std::endl;
		std::cin >> index;
	} while (index < -1 || index + 1 >= automatas.getSize() + 1);

	if (index == -1) {
		return;
	}

	std::cout << "1. Add as new NFA" << std::endl;
	std::cout << "2. replace NFA at index" << std::endl;
	int choice = 0;
	do {
		std::cin >> choice;
	} while (choice != 1 && choice != 2);

	if (choice == 1) {
		automatas.push_back(Star(automatas[index]));
	}
	if (choice == 2) {
		StarRef(automatas[index]);
	}
}

void NFATool::complement() {
	int index = 0;
	do {
		std::cout << "Enter index: (-1 to go back)" << std::endl;
		std::cin >> index;
	} while (index < -1 || index + 1 >= automatas.getSize() + 1);

	if (index == -1) {
		return;
	}

	std::cout << "1. Add as new NFA" << std::endl;
	std::cout << "2. replace NFA at index" << std::endl;
	int choice = 0;
	do {
		std::cin >> choice;
	} while (choice != 1 && choice != 2);

	if (choice == 1) {
		automatas.push_back(Complement(automatas[index]));
	}
	if (choice == 2) {
		automatas[index] = Complement(automatas[index]);
	}
}

void NFATool::intersect() {
	int index = getFirstIndex();
	if (index == -1) {
		return;
	}

	int index1 = getSecondIndex();
	int choice = getChoice();

	if (choice == 1) {
		automatas.push_back(Intersection(automatas[index], automatas[index1]));
	}
	if (choice == 2) {
		automatas[index] = Intersection(automatas[index], automatas[index1]);
	}
	if (choice == 3) {
		automatas[index1] = Intersection(automatas[index], automatas[index1]);
	}
}

void NFATool::difference() {
	int index = getFirstIndex();
	if (index == -1) {
		return;
	}

	int index1 = getSecondIndex();
	int choice = getChoice();

	if (choice == 1) {
		automatas.push_back(Difference(automatas[index], automatas[index1]));
	}
	if (choice == 2) {
		automatas[index] = Difference(automatas[index], automatas[index1]);
	}
	if (choice == 3) {
		automatas[index1] = Difference(automatas[index], automatas[index1]);
	}
}

void NFATool::unite() {
	int index = getFirstIndex();
	if (index == -1) {
		return;
	}

	int index1 = getSecondIndex();
	int choice = getChoice();

	if (choice == 1) {
		automatas.push_back(Union(automatas[index], automatas[index1]));
	}
	if (choice == 2) {
		automatas[index] = Union(automatas[index], automatas[index1]);
	}
	if (choice == 3) {
		automatas[index1] = Union(automatas[index], automatas[index1]);
	}
}

void NFATool::concatenation() {
	int index = getFirstIndex();
	if (index == -1) {
		return;
	}

	int index1 = getSecondIndex();
	int choice = getChoice();

	if (choice == 1) {
		automatas.push_back(Concat(automatas[index], automatas[index1]));
	}
	if (choice == 2) {
		automatas[index] = Concat(automatas[index], automatas[index1]);
	}
	if (choice == 3) {
		automatas[index1] = Concat(automatas[index], automatas[index1]);
	}
}

int NFATool::getFirstIndex() {
	int index = 0;
	do {
		std::cout << "Enter first index: (-1 to go back)" << std::endl;
		std::cin >> index;
	} while (index < -1 || index + 1 >= automatas.getSize() + 1);
	return index;
}

int NFATool::getSecondIndex() {
	int index = 0;
	do {
		std::cout << "Enter second index:" << std::endl;
		std::cin >> index;
	} while (index < 0 || index >= automatas.getSize());
	return index;
}

int NFATool::getChoice() {
	std::cout << "1. Add as new NFA" << std::endl;
	std::cout << "2. replace at first index" << std::endl;
	std::cout << "3. replace at second index" << std::endl;

	int choice = 0;
	do {
		std::cin >> choice;
	} while (choice != 1 && choice != 2 && choice != 3);
	return choice;
}