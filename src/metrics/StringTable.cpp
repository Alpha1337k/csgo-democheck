#include <demo.hpp>

ParsedStringTable::ParsedStringTable(CreateStringTable &st): origin(st)
{
	if (st.name() == "userinfo")
	{
		Update(st.string_data(), st.name() == "userinfo", st.num_entries());
		exit(9);
	}
}


ParsedStringTable::~ParsedStringTable() {}

void	ParsedStringTable::Update(CreateStringTable &st)
{
	Update(st.string_data(), st.name() == "userinfo", st.num_entries());
}

void	ParsedStringTable::Update(UpdateStringTable &ut, bool isUserInfo)
{
	Update(ut.string_data(), isUserInfo, ut.num_changed_entries());
}

void	ParsedStringTable::Update(const std::string &data, bool isUserInfo, int num_changed_entries)
{
#define readBits(x) readStringBits(data, x, i, bitsAvailable)

	int i = 0;
	int lastEntry = -1;
	int lastDictionaryIndex = -1;
	char bitsAvailable = 8;

	// perform integer log2() to set nEntryBits
	int nTemp = origin.max_entries();
	int nEntryBits = 0;
	while (nTemp)
	{
		nTemp >>= 1;
		++nEntryBits;
	}

	std::cout << "Entrybits" << nEntryBits << std::endl;
	//exit(0);

	bool isDictionary = readBits(1) != 0;

	if (isDictionary)
		ErrorReturnMessage("Error: StringTable: encoded using dictionary")

	std::vector< std::string > history;

	for (size_t x = 0; x < num_changed_entries; x++)
	{
		std::cout << "Start------" << std::endl;
		int entryIndex = lastEntry + 1;

		if (readBits(1) == 0)
		{
			entryIndex = readBits(8);
			std::cout << "new index: " << entryIndex << std::endl;
		}
		std::cout << "Entry index: " << entryIndex << std::endl;
		lastEntry = entryIndex;
		if (entryIndex < 0 || entryIndex >= origin.max_entries())
			ErrorReturnMessage("Error: ParseStringUpdate Error")

		std::string entry;
		if (readBits(1) != 0)
		{
			int copySize = 1024;
			if (readBits(1) != 0)
			{
				int index = readBits(5);
				int copyHistSize = readBits(5);
				std::cout << "ReadBits Called! Whoops" << index << " " << copyHistSize << std::endl;
			}
			char charToAdd = ' ';
			int count = 0;
			while (charToAdd != 0 && charToAdd != '\n' && count < copySize)
			{
				charToAdd = readBits(8);
				entry += charToAdd;
				count++;
			}
			std::cout << "Entry size: " << entry.length() << " "<< entry << std::endl;
		}

		std::string userData;
		int size = 0;

		if (readBits(1) != 0)
		{
			if (origin.user_data_fixed_size())
			{
				size = origin.user_data_size();
				std::cout << "Using fixed!!!!" << std::endl;
			}
			else
			{
				size = readBits(14);
			}
			std::cout << "We have a size: " << size <<  std::endl;
			char charToAdd;
			while (size > userData.size())
			{
				charToAdd = readBits(8);
				userData += charToAdd;
			}
			std::cout << "Userdata size: " << userData.size() << "{ " << userData << "}" << std::endl;
		}
		if (size > 0)
		{
			Player p(userData);
			std::cout << p << std::endl;
		}
		std::cout << "End------ total read: " << i << " of " << data.length() << std::endl;
	}
#undef readBits
}
