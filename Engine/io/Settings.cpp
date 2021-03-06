/*
    Author : David Barnett
    Date   : 03-10-2013
    License: MIT License
*/
#include "Settings.h"

#include <iosfwd>
#include <iostream>

using namespace IO;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "../etc/string.h"


//#define LOADING_VERBOSE


Settings::Settings()
{
	this->loading_flag = SETTING_LOAD_NOTHING;
}

Settings::Settings( SettingsLoadFlags flag )
{
	this->loading_flag = flag;
}

Settings::~Settings()
{
	this->unload();
}

void Settings::unload()
{
	this->stored_settings.clear();
}

/**
 * @brief removes all currently loaded settings
 */
void Settings::clear()
{
	this->stored_settings.clear();
}

/**
 * @brief Checks if a given setting exists, and loads it if flags permit
 * @param header section header
 * @param key name of the setting to be accessed
 * @return true on exists
 */
bool Settings::exists ( std::string header , std::string  key )
{
	if ( this->exists( header ) )
	{
		INISection* section = &( this->stored_settings[header] );
		if ( section->loaded )
		{
			return section->properties.find( key ) != section->properties.end();
		}
		else
		{
			//Load Header's properties?
			if ( this->loading_flag == SETTING_LOAD_ON_REQUEST )
			{
				this->load_section ( header , SETTINGS_DUPLICATES_INGORED );
				return this->exists ( header , key );
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}

/**
 * @brief Checks if a header exits
 * @param header Name of section
 * @return true on success
 */
bool Settings::exists ( std::string header )
{
	return this->stored_settings.find( header ) != this->stored_settings.end();
}

/**
 * @brief Loads settings from a file
 * @param file_name
 * @param flag
 */
void Settings::load( std::string file_name , SettingsDuplicateFlags flag )
{
#ifdef LOADING_VERBOSE

	std::cout << "Loading " << file_name << std::endl;
#endif
	std::fstream file;
	file.open( file_name.c_str() );
	this->file_name = file_name;
	//Check if the file is open
	if ( !( file.is_open() ) )
	{
		return;
	}

	//Seek to the beginning of the file
	file.seekg( 0 , file.beg );
	file.peek();

	//Calculate the offset from the 1st byte to the 0th element
	int sys_error = file.tellg();
	file.seekg( 0, file.end );
	int file_size = ( int )file.tellg();
	file.seekg( 0, file.beg );

	std::string line;
	INISection section;
	section.header_name = "";
	section.loaded = false;
	section.start_index = 0;
#ifdef LOADING_VERBOSE

	std::cout << "sys error is " << sys_error << " Pos " << file.tellg() << std::endl;
	std::cout << "root  S:" << section.start_index;
#endif
	while ( ! file.eof() )
	{
		//Get current position with the sys error offset
		int start_pos = ( int )( file.tellg() ) - sys_error;
		std::getline( file , line );

		if ( line.length() == 0 )
			continue;

		line = etc::trim( line );

		//Treat lines starting with ; as comments and do not process
		if ( etc::startswith( line , ";" ) )
		{
			continue;
		}
		//Check if the line has the beginning of a section
		if ( etc::startswith( line , "[" ) && etc::endswith( line , "]" ) )
		{
			//Close last section
			section.end_index = ( int )file.tellg();
			this->stored_settings[section.header_name] = section;
#ifdef LOADING_VERBOSE
			std::cout << " E:" << section.end_index << std::endl;
#endif

			//Refresh section
			section = INISection();
			//Remove the brackets
			section.header_name = line.substr ( 1 , line.size() - 2 );

			if ( this->exists( section.header_name ) )
			{
				//Warning text
				std::cout << "ERROR : Another " << this->file_name << "::" << section.header_name << " has been defined!\nSections will be damaged." << std::endl;
			}
			else
			{
				//-2 constant is to correct the position of the start index
				section.start_index = start_pos ;
				section.loaded = false;
				section.properties = SettingsMap();

#ifdef LOADING_VERBOSE
				std::cout << section.header_name  << " S:" << section.start_index;
#endif


			}
			continue;
		}
	}

	section.end_index = file_size;
	this->stored_settings[section.header_name] = section;

#ifdef LOADING_VERBOSE
	std::cout << " E:" << section.end_index << std::endl;
#endif


	file.close();
}

/**
 * @brief Loads a section's data into memory
 * @param header Section name
 * @param flag Settings Duplicate Flags
 */
void Settings::load_section ( std::string header , SettingsDuplicateFlags flag )
{
	std::fstream file;

	if ( this->exists( header ) == false )
		return;

	INISection* section = &( this->stored_settings[header] );

	file.open( this->file_name.c_str() );
	int file_pos_max = section->end_index;
	//Check if the file is open
	if ( !( file.is_open() ) )
	{
		return;
	}

	//Seek to the beginning of the file
	file.seekg( 0 , file.beg );
	file.peek();


	file.seekg( section->start_index , file.beg );

	bool start = false;
	while ( file.tellg() < file_pos_max && !file.eof() )
	{
		std::string line;
		std::getline ( file, line );
		line = etc::trim ( line );

		if ( line == "" )
		{
			continue;
		}
		if ( etc::startswith( line , ";" ) )
		{
			continue;
		}
		//Check if the line has the beginning of a section
		if ( etc::startswith( line , "[" ) || etc::endswith( line , "]" ) )
		{
			std::string header_name = line.substr ( 1 , line.size() - 2 );
			//If another section is in contact, LEAVE NOW
			if ( header_name != section->header_name )
			{
				break;
			}
			else
			{
				//Found the start of the section we are looking for
				start = true;
			}
			continue;
		}

		if ( start == false )
		{
			continue;
		}

		//Find the equal part of the line
		std::size_t equal_pos = line.find_first_of( '=', 0 );

		if ( equal_pos == line.npos )
		{
			continue;
		}

		std::string key = etc::trim ( line.substr( 0 , equal_pos ) );
		std::string value = etc::trim ( line.substr( equal_pos + 1  , line.length() - 1 ) );

		if ( key == "" )
		{
			continue;
		}

		if ( section->properties.find( key ) !=  section->properties.end() )
		{
			//There is another copy of the key, check if it is OK to override
			if ( flag == SETTINGS_DUPLICATES_OVERRIDE )
			{
				section->properties[key] = value;
			}
		}
		else
		{
			section->properties[key] = value;
#ifdef LOADING_VERBOSE

			std::cout << section->header_name << "::" << key << "=" << value << std::endl;
#endif
		}
	}
	//Update loaded status
	section->loaded = true;
	file.close();

}

/**
 * @brief Unloads a specified section
 * @param header a section name
 */
void Settings::unload_section ( std::string header )
{
	if ( this->exists( header ) )
	{
		this->stored_settings[header].properties.clear();
		this->stored_settings[header].loaded = false;
	}
}

/**
 * @brief Gets the string of the given setting and copies it into the out string
 * @param header the name of the section
 * @param key the name of setting
 * @param out A string to be written to
 * @return true on successful copy
 */
bool Settings::get ( std::string header , std::string  key, std::string* out )
{
	if ( this->exists( header , key ) )
	{
		*out = this->stored_settings[header].properties[key];
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Gets the value and converts it to a bool
 * @see Settings::get
 * @param bol Returns the value
 * @return Returns true if the value is a boolean
 */
bool Settings::getBool ( std::string header , std::string  key , bool* bol )
{
	std::string b;
	if ( this->get( header, key , &b ) == false )
		return false;

	b = etc::toLower( b );
	if ( b == "true" || b == "false" )
	{
		*bol = ( b == "true" ? true : false );
		return true;
	}
	else
	{
		return false;
	}
}

bool Settings::getInt ( std::string header , std::string  key , int* num )
{
	std::string b;
	if ( this->get( header, key , &b ) == false )
		return false;

	if ( etc::is_number( b ) )
	{
		*num = atoi( b.c_str() );
		return true;
	}
	else
	{
		return false;
	}
}

bool Settings::add ( std::string header , std::string key , std::string value )
{
	if ( !this->exists( header , key ) )
	{
		this->stored_settings[header].properties[key] = value;
		return true;
	}
	else
	{
		return false;
	}
}

bool Settings::set ( std::string header , std::string key , std::string value )
{
	if ( this->exists( header , key ) )
	{
		this->stored_settings[header].properties[key] = value;
		return true;
	}
	return false;
}

/**
 * @brief Saves all the sections of a given file
 * @warning Deletes original file and then renames a temp file to replace it
 * @param filename Saves all headers that belong to a file
 * @return bool true on success
 */
bool Settings::save ()
{
	//The File to be read from
	std::ifstream file_in;
	//The File to be written to
	std::fstream file_out;
	//Stream are separate to prevent cross-contamination

	//Open the input file
	file_in.open( this->file_name.c_str() );

	//Check for errors
	if ( file_in.is_open() == false )
	{
		std::cout << "Error input file : " << this->file_name.c_str() << " could not be opened." << std::endl;
		return false;
	}

	//Open the output file
	file_out.open( ( "temp~" + this->file_name ).c_str() , std::ofstream::out );

	//Check for errors
	if ( file_out.is_open() == false )
	{
		std::cout << "Error output file : " << ( "temp~" + this->file_name ).c_str() << " could not be opened." << std::endl;
		return false;
	}

	//Go through whole file
	INISection* current_section = &( this->stored_settings[""] );
	while ( file_in.eof() == false )
	{
		std::string line;
		std::getline ( file_in , line );
		line = etc::trim ( line );

		std::size_t equal_pos = line.find_first_of( '=', 0 );

		if ( line == "" )
		{

		}
		else if ( etc::startswith( line , ";" ) )
		{

		}
		else if ( etc::startswith( line , "[" ) || etc::endswith( line , "]" ) )
		{
			//Check if the line has the beginning of a section
			//Update current_section
			if ( current_section != nullptr )
			{
				current_section->end_index = file_out.tellg();
			}

			current_section = &( this->stored_settings[ line.substr ( 1 , line.size() - 2 )] );
			current_section->start_index = ( int )file_out.tellg() - 1;
		}
		else if ( equal_pos != line.npos )
		{
			std::string key = etc::trim ( line.substr( 0 , equal_pos ) );
			std::string value = current_section->properties[key];
			if ( value == "" )
			{
				value = etc::trim ( line.substr( equal_pos + 1  , line.length() - 1 ) );
			}

			file_out << key << '=' << value << std::endl;
			continue;
		}


		//Standard copy line out
		file_out << line << std::endl;
	}

	file_out.close();
	file_in.close();


	//File operations
	//File operation return value
	int file_op = 0;

	//Remove old version
	file_op = std::remove( this->file_name.c_str() );

	//Check if there was an error
	if ( file_op != 0 )
	{
		std::cout << "Error while removing the file " << this->file_name.c_str() << std::endl;
		return false;
	}

	//Rename the temp version to replace the deleted version
	file_op = std::rename( ( "temp~" + this->file_name ).c_str()  , this->file_name.c_str() );

	//Check if there was an error
	if ( file_op != 0 )
	{
		std::cout << "Error while renaming the file from " << ( "temp~" + this->file_name ).c_str() << " to " << this->file_name.c_str() << std::endl;
		return false;
	}

	return true;
}

std::map<std::string, INISection>* Settings::getStoredSettings ()
{
	return &this->stored_settings;
}
