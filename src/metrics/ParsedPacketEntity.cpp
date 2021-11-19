#include <demo.hpp>

#define readBits(x) readStringBits(data, x, i, bitsAvailable)

DataTable::ServiceClass	PVSParser(const std::string &data, int &i, char &bitsAvailable, int &id, const DataTable &dt)
{
	int serverClassId = readBits(5);

	std::cout << "serverClassId: " << serverClassId << std::endl;

	readBits(10);
	
	DataTable::ServiceClass nSC = DataTable::ServiceClass(dt.services[serverClassId]);

	std::cout << "New Entity: { id: " << nSC.id << ", name: " << nSC.name \
		<< ", tableName: " << nSC.nameDataTable << std::endl;

	return nSC;
}

void	ApplyUpdate(DataTable::ServiceClass serviceClass, const std::string &data, int &i, char &bitsAvailable)
{
	bool readNew = readBits(1);
	int index = -1;
	
}

ParsedPacketEntities::ParsedPacketEntities(PacketEntities &pe, const DataTable &dt)
{
	const std::string &data = pe.entity_data();
	int i = 0;
	char bitsAvailable = 8;
	int currentEntity = -1;

	for (size_t x = 0; x < pe.updated_entries(); x++)
	{
		currentEntity += 1 + readStringVarInt(data, i, bitsAvailable);

		std::cout << "CurrentEn: " << currentEntity << std::endl;
		if (readBits(1) == 0)
		{
			// create
			if (readBits(1))
			{
				std::cerr << "Create" << std::endl;
				DataTable::ServiceClass serviceClass = PVSParser(data, i, bitsAvailable, currentEntity, dt);
				ApplyUpdate(serviceClass, data, i, bitsAvailable);
				exit(1);
			}
			// update
			else
			{
				std::cerr << "Update" << std::endl;
				
				exit(1);
			}
		}
		// delete
		else
		{
			std::cerr << "Delete" << std::endl;
			readBits(1);
			exit(1);
		}


		assert(i < data.length());
	}

}

#undef readBits