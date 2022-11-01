#include <stdio.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctype.h>
using namespace std;
typedef char WORD;
typedef vector<WORD> instruction_t;
typedef instruction_t::iterator instruction_pointer;
typedef vector<WORD> memory_t;
typedef memory_t::iterator memory_pointer;
struct environment{
	memory_t memory;
	instruction_t instructions;
	instruction_pointer ip;
	memory_pointer mp;
	environment() : memory(3000){
		clear();
		mp = memory.begin();
	}
	void clear(){
		instructions.clear();
		ip = instructions.begin();
	}
};
void interpret(environment &env);
int from_line(environment &env, string &line, int open_brackets = 0);
int from_stream(environment &env, istream &stream, int open_brackets = 0);
void interactive_mode(environment &env);
void print_word(WORD word);
int main(int argc, char** argv){
	environment env;
	if(argc == 1){
		interactive_mode(env);
	}
	else{
		ifstream stream = ifstream(argv[1]);
		if(stream.is_open())
		{
			int open_brackets = from_stream(env, stream);
			if(open_brackets == 0)
				interpret(env);
			else
				cout << "Unmatched brackets!" << endl;
		}
		else{
			cout << "File not found : " << argv[1] << endl;
		}
	}
	return 0;
}
void print_word(WORD word){
	if(isprint(word))
		cout << word;
	else
		cout << "0x" << hex << (int)word << dec;
}
int from_line(environment &env, string &line, int open_brackets){
	istringstream stream = istringstream(line);
	return from_stream(env, stream, open_brackets);
}
int from_stream(environment &env, istream &stream, int open_brackets){
	if(open_brackets == 0) 
		env.instructions.push_back('\0');
	WORD word;
	while(true){
		stream >> word;
		if(!stream) break;
		switch(word){
			case '<': case '>': case '+': case '-': case ',': case '.': 
				env.instructions.push_back(word);
				break;
			case '[': 
				open_brackets++;
				env.instructions.push_back(word);
				break;
			case ']':
				open_brackets--;
				env.instructions.push_back(word);
				break;
			default:
				break;
		}
	}
	if(open_brackets == 0)
		env.instructions.push_back('\0');
	return open_brackets;
}
void interactive_mode(environment &env){
	int open_brackets = 0;
	while(true){
		string line;
		getline(cin, line);
		if(!cin) break;

		if(open_brackets == 0) env.clear(); 	
		open_brackets = from_line(env, line, open_brackets);

		if(open_brackets == 0){
			interpret(env);
			cout << ">";
			print_word(*env.mp);
			cout << endl;
		}
		else{
			cout << "->";
		}
	}
}
void find_closing(environment &env){
	int balance = 1;
	do{	
		env.ip++;
		if(*env.ip == '[')
			balance++;
		else if(*env.ip == ']')
			balance--;
		
	}while(balance != 0);
}
void find_opening(environment &env){
	int balance = 0;
	do{
		if(*env.ip == '[')
			balance++;
		else if(*env.ip == ']')
			balance--;
		env.ip--;
	}while(balance != 0);
}
void interpret(environment &env){
	env.ip = env.instructions.begin();
	while(env.ip != env.instructions.end()){
		switch(*env.ip){
		case '+':
			(*env.mp)++;
			env.ip++;
			break;
		case '-':
			(*env.mp)--;
			env.ip++;
			break;
		case '>':
			if(env.mp != (env.memory.end()--))
				env.mp++;
			env.ip++;
			break;
		case '<':
			if(env.mp != env.memory.begin())
				env.mp--;
			env.ip++;
			break;
		case '.':
			print_word(*env.mp);
			env.ip++;
			break;
		case ',':
			WORD word;
			cin >> word;
			(*env.mp) = word;
			env.ip++;
			break;
		case '[':
			if(!(*env.mp))
				find_closing(env);
			env.ip++;
			break;
		case ']':
			find_opening(env);
			env.ip++;
			break;
		case '\0':
			env.ip++;
			break;
		}
	}
}
