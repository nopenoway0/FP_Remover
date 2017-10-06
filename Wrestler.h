#ifndef _WRESTLER_H
#define _WRESTLER_H

#include "Character.h"

namespace fp_remover{ namespace classes{

enum FIELD_FLAGS
{
	WEIGHT = 1,
	HEIGHT = WEIGHT << 1,
	E_POINTS = HEIGHT << 1,
	B_YEAR = E_POINTS << 1,
	F_NAME = B_YEAR << 1,
	L_NAME = F_NAME << 1,
	N_NAME = L_NAME << 1,
	D_NAME = N_NAME << 1,
	G_ID = D_NAME << 1,
};

using namespace std;

class Wrestler : public Character
{
public:
	Wrestler();
	Wrestler(string name);
	void set_field(unsigned short in, int field);
	void set_field(string in, int flag);
	void set_field(int in, int field);
	int get_groupID();
private:
	string first_name;
	string last_name;
	string nickname;
	unsigned short weight;
	unsigned short height;
	unsigned short edit_points;
	unsigned short birth_year;
	int group_id;
};

}}

#endif