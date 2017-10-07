#ifndef _WRESTLER_LOADER_H
#define _WRESTLER_LOADER_H

#include "SaveLoader.h"
#include "Wrestler.h"

namespace fp_remover{ namespace loaders{

using frontend::loaders::SaveLoader;
using fp_remover::classes::Wrestler;
using namespace std;

class WrestlerLoader : public SaveLoader<Wrestler>
{
public:
	WrestlerLoader();
	WrestlerLoader(string name);
	void load() override;
	Wrestler parse() override;
private:
	// inherit:
	// vector<T> contents
	// ifstream* f
};

}}
#endif