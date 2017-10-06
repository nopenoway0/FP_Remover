#include "Wrestler.h"

namespace fp_remover{ namespace classes{

using namespace std;

Wrestler::Wrestler(string name) : Character(name)
{

}


Wrestler::Wrestler() : Character()
{

}

void Wrestler::set_field(unsigned short in, int flag)
{
	if(flag & WEIGHT) this->weight = in;
	else if(flag & HEIGHT) this->height = in;
	else if(flag & E_POINTS) this->edit_points = in;
	else if(flag & B_YEAR) this->birth_year = in;
	// throw exception if not
}

void Wrestler::set_field(int in, int flag)
{
	if(flag & G_ID) this->group_id = in;
}

void Wrestler::set_field(string in, int flag)
{
	if(flag & F_NAME) this->first_name = in;
	else if(flag & L_NAME) this->last_name = in;
	else if(flag & N_NAME) this->nickname = in;
	this->set_name(this->first_name + " " + this->last_name + " - " + nickname);
}

int Wrestler::get_groupID()
{
	return this->group_id;
}

}}