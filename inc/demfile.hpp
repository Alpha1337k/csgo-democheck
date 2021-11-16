#ifndef DEMFILE_HPP
#define DEMFILE_HPP

#include <vector>
#include <stdio.h>
#include <exception>
#include <netmessages.pb.h>
#include <csgomsg.pb.h>

#define ErrorReturnMessage(msg) \
	{ std::cerr << msg << std::endl; return; }
#define ReturnMessage(msg) \
	{ std::cout << msg << std::endl; return; }

#define MessageVector std::vector<std::pair<int, void *>>

enum PacketTypes
{
	// it's a startup message, process as fast as possible
	dem_signon	= 1,
	// it's a normal network packet that we stored off
	dem_packet,
	// sync client clock to demo tick
	dem_synctick,
	// console command
	dem_consolecmd,
	// user input command
	dem_usercmd,
	// network data tables
	dem_datatables,
	// end of time.
	dem_stop,
	// a blob of binary data understood by a callback function
	dem_customdata,

	dem_stringtables,

	// Last command
	dem_lastcmd		= dem_stringtables,
};

enum PreParsedPackages
{
	svc_DataTable = -1
};

struct Vector
{
	float x, y, z;
	void Init( void );
	void Init( float _x, float _y, float _z );
};
std::ostream &operator<<(std::ostream &o, const Vector &d);

struct DemHeader
{
	char 	headerDef[8];
	int		demoProtocol;
	unsigned int		networkProtocol;
	char	serverName[260];
	char	clientName[260];
	char	mapName[260];
	char	gameDir[260];
	float	demoLength;
	int		ticks;
	int		frames;
	int		signOnLength;

};
std::ostream &operator<<(std::ostream &o, const DemHeader &d);

struct Packet
{
	struct Split_t
	{
		int			flags;

			// original origin/viewangles
		Vector		viewOrigin;
		Vector		viewAngles;
		Vector		localViewAngles;

			// Resampled origin/viewangles
		Vector		viewOrigin2;
		Vector		viewAngles2;
		Vector		localViewAngles2;
	};
	Split_t splits[2];
	MessageVector msg;
	Packet(FILE *f);
	Packet();
};
std::ostream &operator<<(std::ostream &o, const Packet &d);

struct Frame
{
	unsigned char	cmd;
	int				tick;
	char			playerslot;

	Packet			pckt;
	Frame(FILE *f, bool &finished);
};
std::ostream &operator<<(std::ostream &o, const Frame &d);

struct DataTable
{
	struct ServiceClass
	{
		short id;
		std::string name;
		std::string nameDataTable;
	};
	std::vector<ServiceClass> services;
	MessageVector msg;

	DataTable(FILE *f);
};

struct Player {
	long	version;
	long	xUid;
	char	userName[128];
	int		userId;
	char	gUid[33];
	int		friendsId;
	char	friendsName[128];
	bool	isFake;
	bool	isHltv;

	int		customFiles[4];
	char	filesDownloaded;
};

class ParsedStringTable
{
private:
	void Update(const std::string &data, bool isUserInfo, int num_changed_entries);
public:
	CreateStringTable &origin;
	ParsedStringTable(CreateStringTable &st);
	void	Update(CreateStringTable &st);
	void	Update(UpdateStringTable &ut, bool isUserInfo = false);
	~ParsedStringTable();
};


class DemoFile
{
private:
	DemHeader header;
	ServerInfo *info;
	std::string	signOnData;
	std::vector<Frame> frames;
	std::vector<GameEventList_descriptor_t> gEvents;
	std::vector<ParsedStringTable> sTables;
	std::vector<Player>				players;

	std::vector<MessageVector> ParseRounds();

	void handleGameEventList(GameEventList &ge);
	void handleGameEvent(GameEvent &ge);
	void handleServerInfo(ServerInfo &si);
	void handleCreateStringTable(CreateStringTable &si);
	void handleUpdateStringTable(UpdateStringTable &si);
	void handlePacketEntities(PacketEntities &e);
	void handleUserMessage(UserMessage &e);
	void handleDataTable(DataTable &dt);
public:
	DemoFile(FILE *f);
	~DemoFile();
	void	create_metrics();
};

struct GameEventListParsed
{

	GameEventListParsed(GameEventList &l);
};

struct GameEventParsed
{

	GameEventParsed(GameEvent &l);
};

std::string	readVarString(FILE *f, size_t *iter);
std::string	readVarString(const std::string &str, int &iter);
int readVarInt(FILE *f, size_t *iter);
bool	readVarBool(FILE *f, size_t *iter);
MessageVector getProtoMesssages(FILE *f, int size);
int readVarInt(char *ar, size_t *iter);
int		readStringBits(const std::string &str, int count, int &i, char &bitsAvailable);

#endif