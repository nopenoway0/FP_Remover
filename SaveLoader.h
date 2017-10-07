#ifndef _SAVELOADER_H
#define _SAVELOADER_H

#include <fstream>
#include <vector>
namespace frontend{ namespace loaders{

using namespace std;

template <class T>
class  SaveLoader
{
public:	
	SaveLoader()
	{
		f = nullptr;
	}
	SaveLoader(string filename)
	{
		if(!this->open_file(filename)) f = nullptr;
	}
	~SaveLoader()
	{
		if(f->is_open()) f->close();
		contents.clear();
		delete f;
	}

	/**
	 * Opens file according to string name and sets stream in object to
	 * corrersponding ifstream
	 * @param  filename
	 * @return true | false whether ifstream is open
	 */
	bool open_file(string filename)
	{
		f = new ifstream(filename, ios_base::binary);
		return f->is_open();
	}
	/**
	 * loads entirety of contents vector
	 */
	virtual void load()=0;

	/**
	 * parses file for next T object
	 * @return next T object from file
	 */
	virtual T parse()=0;
protected:
	ifstream* f;
	vector<T> contents;
};

}}
#endif