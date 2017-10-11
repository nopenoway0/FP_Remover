#include "WrestlerLoader.h"

namespace fp_remover{ namespace loaders{

	int NUM_CHUNKS = 17;

	using namespace std;

	typedef struct saveDataChunk{
		int32_t chunkKind;
		int32_t version;
		int64_t top;
	} saveDataChunk;

	enum
	{
		F_NAME,
		L_NAME,
		NICKNAME,
	};

	typedef fp_remover::classes::FIELD_FLAGS wrestler_flags;

	WrestlerLoader::WrestlerLoader() : SaveLoader()
	{
		this->file_memory = nullptr;
		this->chunk_location = nullptr;
	}

	WrestlerLoader::WrestlerLoader(string filename) : SaveLoader(filename)
	{
		this->file_memory = nullptr;
		this->chunk_location = nullptr;
	}

	void WrestlerLoader::load()
	{
		if(f == nullptr || !f->is_open())
		{
			string error_msg("Invalid File: " + filename);
			throw runtime_error(error_msg);
		}
		int num_chunks, number_wrestlers, counter = 0;
		char *buffer;
		saveDataChunk* list_chunks;

		this->clear_memory(); // clear all pointers to avoid memory leaks

		cout << "memory cleared" << endl;

		f->seekg(0, ios_base::end);
		filesize = f->tellg();
		f->seekg(ios_base::beg);

		file_memory = (char*) malloc(sizeof(char) * filesize);

		f->read(file_memory, filesize);

		string read_string;
		buffer = new char[16];

		f->seekg(0L);

		// read 2 ints, second is number of chunks
		f->read(buffer, 8);
		num_chunks = *(int32_t*) &buffer[4];

		cout << "loading " << num_chunks << " memory chunks" << endl;
		if(num_chunks != NUM_CHUNKS)
		{
			for(int x = 0; x < 16; x++)
			{
				cout << *(int32_t*) &buffer[x] << endl;
			}
			throw runtime_error("invalid number of chunks");
		} 

		list_chunks = new saveDataChunk[num_chunks];

		f->seekg(16L); // move to location to begin loading chunks
		chunk_location = new long[num_chunks]; // store chunk top location for changing after wrestler is removed

		for(int x = 0; x < num_chunks; x++)
		{
			f->read(buffer, 16);
			list_chunks[x].chunkKind = *(int32_t*) &buffer[0];
			list_chunks[x].version = *(int32_t*) &buffer[4];
			list_chunks[x].top = *(int64_t*) &buffer[8];


			chunk_location[x] = (long) f->tellg() - 8;
		}

	 	// start wrestling load
		f->seekg(list_chunks[4].top);

		this->wrestler_record_loc = f->tellg();
		f->read(buffer, 4);
		number_wrestlers = *(int*) buffer;

		// does x amount of wreslers
		Wrestler* wr;
		for(int x = 0; x < number_wrestlers; x++)
		{
			wr = new Wrestler();
			wr->set_fp_start(f->tellg());
			counter = 0;
			for(int y = 0; y < 3; y++)
			{
				f->read(buffer, 4);
				counter++;
			}

			f->read(buffer, 1);
			counter++;

			// get names
			for(int y = 0; y < 4; y++)
			{
				read_string = getString();
				switch(y)
				{
					case F_NAME:	wr->set_field(read_string, wrestler_flags::F_NAME);
									break;
					case L_NAME: 	wr->set_field(read_string, wrestler_flags::L_NAME);
									break;
					case NICKNAME:	wr->set_field(read_string, wrestler_flags::N_NAME);
				}
				counter++;

			}

			for(int y = 0; y < 27; y++)
			{
				f->read(buffer, 4);
				counter++;
				if(counter == 12) break; // break at birth year starts at this point
			}

			f->seekg(2 * 4, f->cur);

			f->read(buffer, 4);
			wr->set_field(*(int*) buffer, wrestler_flags::G_ID);

			f->seekg((324 * 4) + 1 + (91 * 4 * 3), f->cur); // skip unecessary values
			
			read_string = getString();
			f->seekg(17, f->cur); // skip 17 bytes
			read_string = getString();

			// costume data loading
			f->seekg(12, f->cur);

			for(int y = 0; y < 4; y++)
			{
				f->seekg(1, f->cur); // skip bool
				for(int z = 0; z < 144; z++) read_string = getString();
				f->seekg((144 * 4 * 4) + (149 * 4), f->cur); // skip 144 * 4 float then 149 float
			}

			wr->set_fp_end(f->tellg());
			this->contents.push_back(*wr);
			delete wr;
		}

		f->seekg(list_chunks[10].top + 4);

		f->read(buffer, 4);
		int num_elements = *(int*) buffer;
		cout << "finding possible id" << endl;
		for(int x = 0; x < num_elements; x++)
		{
			f->read(buffer, 4);
			if(*(int*) buffer >= 10000)
			{
				cout << *(int*) buffer << endl;
				f->seekg(-1*4, f->cur);
				break;
			}
		}
		//f->seekg((num_elements - 1) * 4, f->cur);

		this->byte_skip_loc = f->tellg();

		delete [] list_chunks;
		delete [] buffer;
		wr = nullptr;
	}
	
	Wrestler WrestlerLoader::parse()
	{
		throw runtime_error("UNIMPLEMENTED METHOD: parse");
		return Wrestler();
	}

	int WrestlerLoader::getLength()
	{
	    int count = 0;
	    int shift = 0;
	    char b;
	    do {
	        if (shift == 5 * 7)
	        {
	        	//cerr << "error reading string" << endl;
	        }

	        // ReadByte handles end of stream cases for us.
	        this->f->read(&b, 1);
	        count |= (b & 0x7F) << shift;
	        shift += 7;
	    } while ((b & 0x80) != 0);
	    return count;
	}

	string WrestlerLoader::getString()
	{
		int length = this->getLength();
		char* buffer = new char[length + 1]();
		f->read(buffer, length);
		buffer[length] = '\0';
		string result(buffer);
		delete [] buffer;
		return result;
	}

	void WrestlerLoader::clear_memory()
	{
		if(file_memory != nullptr)
		{
			free(file_memory);
			file_memory = nullptr;
		}
		if(chunk_location != nullptr)
		{
			delete [] chunk_location;
			chunk_location = nullptr;
		}
		this->contents.clear();

	}

	WrestlerLoader::~WrestlerLoader()
	{
		free(file_memory);
		delete [] chunk_location;
	}

	vector<Wrestler> WrestlerLoader::get_wrestlers()
	{
		return contents;
	}

	int WrestlerLoader::get_record_loc()
	{
		return this->wrestler_record_loc;
	}

	unsigned long WrestlerLoader::get_byte_skip_loc()
	{
		return this->byte_skip_loc;
	}

	const char* WrestlerLoader::get_file_p()
	{
		const char *p = &file_memory[0];
		return p;
	}

	unsigned long WrestlerLoader::get_filesize()
	{
		return this->filesize;
	}

	const long int* WrestlerLoader::get_chunk_locations()
	{
		const long int* p = &chunk_location[0];
		return p;
	}

	bool WrestlerLoader::write(char* b_in, int size)
	{
		f->close();
		f->open(this->filename, ios_base::binary | ios_base::in | ios_base::out | ios_base::trunc);
		if(!f->is_open()) throw runtime_error("invalid file");
		f->write(b_in, size);
		f->close();
		open_file(filename);
		return true;
	}

}}