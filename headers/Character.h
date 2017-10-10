#ifndef _CHARACTER_H
#define _CHARACTER_H

#include <iostream>

namespace fp_remover{ namespace classes{

using namespace std;
class Character
{

public:
	Character();
	Character(string name);
	~Character();
	string get_name();
	void set_name(string name);
	long get_fp_start();
	long get_fp_end();
	void set_fp_start(long in);
	void set_fp_end(long in);
protected:
	long character_fp_start;
	long character_fp_end;
	string name;

};

}}


#endif