#include "config.h"

CConfiguration::CConfiguration()
{
	this->_inih				= nullptr;
	this->configOpened		= false;
	this->configError		= false;
	this->configFilename	= DEFAULT_CONFIG;
	
	this->_inih = new INIReader(this->configFilename);
	if (this->GetReader()->ParseError() < 0) {
		printf("[CConfiguration] %s could not be loaded.\n", this->configFilename.c_str());
		this->configError = true;
	}
	else {
		printf("[CConfiguration] %s loaded successfully.\n", this->configFilename.c_str());
		this->configOpened = true;
	}
}
CConfiguration::~CConfiguration()
{
	printf("[CConfiguration] CConfiguration shutting down\n");
}
bool CConfiguration::PopulateValues()
{
	if (this->configError)
		return false;

	/* Populate configuration values from INI */
	std::string tmp = this->GetReader()->Get("General", "III Dir", "");
	strcpy(this->IIILocation, tmp.c_str());

	tmp = this->GetReader()->Get("General", "VC Dir", "");
	strcpy(this->VCLocation, tmp.c_str());
	

	tmp = this->GetReader()->Get("General", "SA Dir", "");
	strcpy(this->SALocation, tmp.c_str());

	return true;
}
std::string CConfiguration::sections(INIReader &reader)
{
	std::stringstream ss;
	std::set<std::string> sections = reader.Sections();
	for (std::set<std::string>::iterator it = sections.begin(); it != sections.end(); ++it)
		ss << *it << ",";
	return ss.str();
}