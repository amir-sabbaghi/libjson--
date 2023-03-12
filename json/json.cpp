#include "json.h"
#include <stdio.h>
#include <fstream>

using namespace std;

Json::Json()
{
	type_ = Json::JTYPE_NULL;
}

Json::Json(const Json& json)
{
	type_ = json.type_;
	for (std::map<std::string, Json *>::const_iterator i = json.children_.begin(); i != json.children_.end(); i ++)
		children_.insert(std::pair<std::string, Json *>(i->first, new Json(*i->second)));
	for (std::vector<Json *>::const_iterator i = json.array_.begin(); i != json.array_.end(); i ++)
		array_.push_back(new Json(**i));
	value_ = json.value_;
}

Json::Json(const std::string& path)
{
	load(path);
}

Json::~Json()
{
	for (std::map<std::string, Json *>::iterator i = children_.begin(); i != children_.end(); i ++)
		delete i->second;
	for (std::vector<Json *>::iterator i = array_.begin(); i != array_.end(); i ++)
		delete *i;
}

void Json::load(const std::string& path)
{
	clear();
	FILE *f = fopen(path.c_str(), "r");
	if (!f)
		throw "Json error: cannot open file";
	fseek(f, 0, SEEK_END);
	unsigned size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *text = new char[size+1];
	unsigned count = fread(text, 1, size, f);
	fclose(f);
	if (count < size)
	{
#ifndef _WIN32
		throw "Json error: file is corrupted";
#endif
	}
	text[count] = 0;
	rec_parse(text);
	delete []text;
}

void Json::save(const std::string& path) const
{
	ofstream f(path.c_str());
	rec_write(f);
	f.close();
}

void Json::save(std::string &output) const
{
	stringstream ss;
	rec_write(ss, 0);
	output = ss.str();
}

void Json::parse(const std::string& text)
{
	rec_parse(text.c_str());
}

void Json::clear()
{
	type_ = Json::JTYPE_NULL;
	for (std::map<std::string, Json *>::iterator i = children_.begin(); i != children_.end(); i ++)
		delete i->second;
	children_.clear();
	for (std::vector<Json *>::iterator i = array_.begin(); i != array_.end(); i ++)
		delete *i;
	array_.clear();
	value_.clear();
}

int Json::rec_parse(const char* const text)
{
#define SKEEP_SPACES \
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') \
		p ++
	
	const char *p = text;
	SKEEP_SPACES;
	if (*p == 0)
		return p - text;
	if (*p == '{')
	{
		type_ = Json::JTYPE_OBJECT;
		while (*p != '}')
		{
			p ++;
			SKEEP_SPACES;
			// Watch out for an empty object
			if (*p == '}')
				break;
			if (*p != '"')
				throw "JSON object member name is not a string"; // FIXME
			string name;
			p += read_string(p, name);
			p ++;
			SKEEP_SPACES;
			if (*p != ':')
				throw "JSON object member name does not followed by a colon"; // FIXME
			p ++;
			SKEEP_SPACES;
			Json *child = new Json;
			p += child->rec_parse(p);
			p ++;
			SKEEP_SPACES;
			if (*p != ',' && *p != '}')
				throw ("\"}\" or \",\" expected after member \"" + name + "\"").c_str(); // FIXME
			children_.insert(pair<string, Json*>(name, child));
		}
	}
	else if (*p == '[')
	{
		type_ = Json::JTYPE_ARRAY;
		while (*p != ']')
		{
			p ++;
			SKEEP_SPACES;
			// Watch out for an empty object
			if (*p == ']')
				break;
			Json *child = new Json;
			p += child->rec_parse(p);
			p ++;
			SKEEP_SPACES;
			if (*p != ',' && *p != ']')
				throw "\"]\" or \",\" expected"; // FIXME
			array_.push_back(child);
		}
	}
	else if (*p == '"')
	{
		type_ = Json::JTYPE_STRING;
		p += read_string(p, value_);
	}
	else
	{
		for (; *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != ',' && *p != ':' && *p != '[' && *p != ']' && *p != '{' && *p != '}'; p ++)
			value_.push_back(*p);
		if (value_ == "null")
			type_ = JTYPE_NULL;
		else if (value_ == "true" || value_ == "false")
			type_ = JTYPE_BOOLEAN;
		else
			type_ = JTYPE_NUMBER;
		p --;
	}
	return p - text;
#undef SKEEP_SPACES
}

int Json::read_string(const char* const text, string& name)
{
	const char *p = text;
	for (p ++; *p && *p != '"'; p ++)
	{
		if (*p == '\\')
		{
			p ++;
			switch (*p)
			{
			case 'b':
				name.push_back('\b');
				break;
			case 'f':
				name.push_back('\f');
				break;
			case 'n':
				name.push_back('\n');
				break;
			case 'r':
				name.push_back('\r');
				break;
			case 't':
				name.push_back('\t');
				break;
			case 'u':
			{
				int num = 0;
				int i;
				for (p ++, i = 0; i < 4 && ((*p >= '0' && *p <= '9') || (tolower(*p) >= 'a' && tolower(*p) <= 'f')); p ++)
					num = num * 16 + (*p >= 'A' ? tolower(*p) - 'a' + 10 : *p - '0');
				break;
			}
			default:
				name.push_back(*p);
			}
		}
		else
			name.push_back(*p);
	}
	return p - text;
}

void Json::rec_write(ostream& f, int tab) const
{
#define INSERT_TABS \
	for (int j = 0; j < tab; j ++) \
		f << '\t'
	
	switch (type_)
	{
	case Json::JTYPE_OBJECT:
	{
		f << "{\n";
		map<string, Json*>::const_iterator i = children_.begin();
		if (i != children_.end())
		{
			INSERT_TABS;
			f << "\t\"" << i->first << "\" : ";
			i->second->rec_write(f, tab+1);
		}
		for (i ++; i != children_.end(); i ++)
		{
			f << ",\n";
			INSERT_TABS;
			f << "\t\"" << i->first << "\" : ";
			i->second->rec_write(f, tab+1);
		}
		f << '\n';
		INSERT_TABS;
		f << '}';
		break;
	}
	case Json::JTYPE_ARRAY:
	{
		f << "[\n";
		vector<Json*>::const_iterator i = array_.begin();
		if (i != array_.end())
		{
			INSERT_TABS;
			f << '\t';
			(*i)->rec_write(f, tab+1);
		}
		for (i ++; i != array_.end(); i ++)
		{
			f << ",\n";
			INSERT_TABS;
			f << '\t';
			(*i)->rec_write(f, tab+1);
		}
		f << '\n';
		INSERT_TABS;
		f << ']';
		break;
	}
	case Json::JTYPE_STRING:
		f << '"' << value_ << '"';
		break;
	case Json::JTYPE_NUMBER:
	case Json::JTYPE_BOOLEAN:
		f << value_;
		break;
	case Json::JTYPE_NULL:
		f << "null";
		break;
	default:
		break;
	}
}

