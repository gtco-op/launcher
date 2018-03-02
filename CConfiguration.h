#pragma once
class CConfiguration
{
public:
	/* GTAIII Location */
	char IIILocation[MAX_PATH];

	/* GTAVC Location */
	char VCLocation[MAX_PATH];

	/* GTASA Location */
	char SALocation[MAX_PATH];

	/* Populates values from INI file */
	bool PopulateValues();
	
	/* Retrieves the INI reader instance */
	INIReader* GetReader() { return _inih; }

	/* Returns TRUE if config has been loaded successfully */
	bool IsConfigLoaded() { return configOpened; }

	bool IsConfigReadable() { return !configError; }

public:
	CConfiguration();
	~CConfiguration();

private:
	INIReader* _inih;
	
	bool configError;

	bool configOpened;
		
	std::string configFilename;

	std::string sections(INIReader &reader);
};

