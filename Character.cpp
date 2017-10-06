#include "Character.h"

namespace fp_remover{ namespace classes{

Character::Character()
{
	character_fp_start = -1;
	character_fp_end = -1;
	name = "n/a";
}

Character::Character(string name)
{
	this->name = name;
	character_fp_end = -1;
	character_fp_start = 1;
}

string Character::get_name()
{
	return this->name;
}

void Character::set_name(string name)
{
	this->name = name;
}

long Character::get_fp_start()
{
	return this->character_fp_start;
}

long Character::get_fp_end()
{
	return this->character_fp_end;
}

void Character::set_fp_start(long in)
{
	this->character_fp_start = in;
}

void Character::set_fp_end(long in)
{
	this->character_fp_end = in;
}

Character::~Character()
{

}

}} 