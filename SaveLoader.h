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
		f = nullptr;
		if(!this->open_file(filename)) f = nullptr;
		this->filename = filename;
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
		//if(f != nullptr) ;
		if(f != nullptr)
		{
			if(f->is_open()) f->close();
			delete f;
		}
		this->filename = filename;
		f = new fstream(filename, ios_base::binary | ios_base::in | ios_base::out);
		if(!f->is_open())
		{
			delete f;
			f = nullptr;
			return false;
		}
		else return true;
	}
	/**
	 * loads entirety of contents vector
	 */
	virtual void load()=0;

	void set_filename(string filename)
	{
		this->filename = filename;
	}

	/**
	 * parses file for next T object
	 * @return next T object from file
	 */
	virtual T parse()=0;
protected:
	string filename;
	fstream* f;
	vector<T> contents;
};

}}
#endif