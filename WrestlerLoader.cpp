#include "WrestlerLoader.h"

namespace fp_remover{ namespace loaders{

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
	}

	WrestlerLoader::WrestlerLoader(string filename) : SaveLoader(filename)
	{
		this->file_memory = nullptr;
	}

	void WrestlerLoader::load()
	{
		int num_chunks, wrestler_num_location, number_wrestlers, counter = 0;
		int* modifier = nullptr;
		char *buffer;
		saveDataChunk* list_chunks;

		f->seekg(0, ios_base::end);
		filesize = f->tellg();
		f->seekg(ios_base::beg);

		if(file_memory != nullptr) delete [] file_memory;
		file_memory = new char[filesize];

		f->read(file_memory, filesize);

		string read_string;
		buffer = new char[16];

		f->seekg(0L);

		// read 2 ints, second is number of chunks
		f->read(buffer, 8);
		num_chunks = *(int32_t*) &buffer[4];

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
/*
		// set new number to wrestlers
		modifier = (int*) &file_memory[this->wrestler_record_loc];
		*modifier = (number_wrestlers - 1);

		// change chunk tops
		for( int x = 5; x < 17; x++)
		{
			modifier = (int*) &file_memory[chunk_location[x]];
			*modifier -= (this->contents[0].get_fp_end() - this->contents[0].get_fp_start());		
		}

		for(int x = 11; x < 17; x++)
		{
			modifier = (int*) &file_memory[chunk_location[x]];
			*modifier -= 4;
		}
*/
		f->seekg(list_chunks[10].top + 4);

		f->read(buffer, 4);
		int num_elements = *(int*) buffer;

		f->seekg((num_elements - 1) * 4, f->cur);

		this->byte_skip_loc = f->tellg();

		// generate class to handle this
		for(long x = 0; x < filesize; x++)
		{
			//if(byte_test_loc == x) x += 3;
			//else if((x <= w_list[0].get_fp_start()) || (x > w_list[0].get_fp_end())) o_f->write(&file_memory[x], 1);
		}

		// end what class should do
		delete [] list_chunks;
		delete [] buffer;
		modifier = nullptr;
		wr = nullptr;
	}
	
	Wrestler WrestlerLoader::parse()
	{

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
		this->f->read(buffer, length);
		buffer[length] = '\0';
		string result(buffer);
		delete [] buffer;
		return result;
	}

	void WrestlerLoader::clear_memory()
	{
		delete [] this->file_memory;
		this->file_memory = nullptr;
	}

	WrestlerLoader::~WrestlerLoader()
	{
		delete [] this->file_memory;
		delete [] this->chunk_location;
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

}}