#ifndef JSON_H
#define JSON_H

#include <string>
#include <map>
#include <vector>
#include <sstream>

class Json
{
public:
	enum Type{
		JTYPE_NULL,
		JTYPE_OBJECT,
		JTYPE_ARRAY,
		JTYPE_STRING,
		JTYPE_NUMBER,
		JTYPE_BOOLEAN
	};

	Json();
	Json(const Json& json);
	Json(const std::string& path);
	virtual ~Json();

	void load(const std::string& path);
	void save(const std::string& path) const;
	void save(std::string& output) const;
	void parse(const std::string& text);
	void clear();

	typedef std::map<std::string, Json*>::iterator ObjectIterator;
	typedef std::map<std::string, Json*>::const_iterator ConstObjectIterator;
	typedef std::vector<Json*>::iterator ArrayIterator;
	typedef std::vector<Json*>::const_iterator ConstArrayIterator;
	inline ObjectIterator objectBegin();
	inline ConstObjectIterator objectBegin() const;
	inline ObjectIterator objectEnd();
	inline ConstObjectIterator objectEnd() const;
	inline ArrayIterator arrayBegin();
	inline ConstArrayIterator arrayBegin() const;
	inline ArrayIterator arrayEnd();
	inline ConstArrayIterator arrayEnd() const;

	inline Type getType() const;
	inline void setType(const Type type);
	inline unsigned size() const;

	// array access
	inline Json& operator [](unsigned index);
	inline const Json& operator [](unsigned index) const;
	// object access
	inline ConstObjectIterator find(const std::string& key) const;
	inline Json& operator [](const std::string& key);
	inline const Json& operator [](const std::string& key) const;

	inline Json& operator=(const Json& json);
	inline Json& operator=(const char* value);
	inline Json& operator=(const std::string& value);
	inline Json& operator=(char value);
	inline Json& operator=(short value);
	inline Json& operator=(int value);
	inline Json& operator=(long value);
	inline Json& operator=(long long value);
	inline Json& operator=(unsigned char value);
	inline Json& operator=(unsigned short value);
	inline Json& operator=(unsigned int value);
	inline Json& operator=(unsigned long value);
	inline Json& operator=(unsigned long long value);
	inline Json& operator=(float value);
	inline Json& operator=(double value);
	inline Json& operator=(long double value);
	inline Json& operator=(bool value);

	template <class T> T as() const;

protected:
	Type type_;
	std::map<std::string, Json*> children_;
	std::vector<Json*> array_;
	std::string value_;

	int rec_parse(const char* const text);
	int read_string(const char* const text, std::string& name);
	void rec_write(std::ostream& f, int tab = 0) const;
};

// inline implementation
Json::ObjectIterator Json::objectBegin()
{
	return children_.begin();
}

Json::ConstObjectIterator Json::objectBegin() const
{
	return children_.begin();
}

Json::ObjectIterator Json::objectEnd()
{
	return children_.end();
}

Json::ConstObjectIterator Json::objectEnd() const
{
	return children_.end();
}

Json::ArrayIterator Json::arrayBegin()
{
	return array_.begin();
}

Json::ConstArrayIterator Json::arrayBegin() const
{
	return array_.begin();
}

Json::ArrayIterator Json::arrayEnd()
{
	return array_.end();
}

Json::ConstArrayIterator Json::arrayEnd() const
{
	return array_.end();
}

Json::Type Json::getType() const
{
	return type_;
}

void Json::setType(const Json::Type type)
{
	if (type_ != type)
	{
		clear();
		type_ = type;
	}
}

unsigned Json::size() const
{
	switch (type_)
	{
	case JTYPE_ARRAY:
		return array_.size();
	case JTYPE_OBJECT:
		return children_.size();
	case JTYPE_NULL:
		return 0;
	default:
		return 1;
	}
}

Json& Json::operator[](unsigned int index)
{
	if (type_ != Json::JTYPE_ARRAY)
	{
		clear();
		type_ = Json::JTYPE_ARRAY;
	}
	while (index >= array_.size())
		array_.push_back(new Json);
	return *array_[index];
}

const Json& Json::operator[](unsigned int index) const
{
	if (type_ != Json::JTYPE_ARRAY)
		throw "This JSON value is not an array"; // FIXME: should throw an appropriate exception
	return *array_[index];
}

Json::ConstObjectIterator Json::find(const std::string& key) const
{
	if (type_ != Json::JTYPE_OBJECT)
		throw "This JSON value is not an object"; // FIXME: should throw an appropriate exception
	return children_.find(key);
}

Json& Json::operator[](const std::string& key)
{
	if (type_ != Json::JTYPE_OBJECT)
	{
		clear();
		type_ = Json::JTYPE_OBJECT;
	}
	if (children_.find(key) == children_.end())
		children_[key] = new Json();
	return *children_[key];
}

const Json& Json::operator[](const std::string& key) const
{
	if (type_ != Json::JTYPE_OBJECT)
		throw "This JSON value is not an object"; // FIXME: should throw an appropriate exception
	return *children_.find(key)->second;
}

Json& Json::operator=(const Json& json)
{
	clear();
	type_ = json.type_;
	for (std::map<std::string, Json *>::const_iterator i = json.children_.begin(); i != json.children_.end(); i ++)
		children_.insert(std::pair<std::string, Json *>(i->first, new Json(*i->second)));
	for (std::vector<Json *>::const_iterator i = json.array_.begin(); i != json.array_.end(); i ++)
		array_.push_back(new Json(**i));
	value_ = json.value_;
	return (*this);
}

Json& Json::operator=(const char* value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	value_ = value;
	type_ = Json::JTYPE_STRING;
	return *this;
}

Json& Json::operator=(const std::string& value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	value_ = value;
	type_ = Json::JTYPE_STRING;
	return *this;
}

Json& Json::operator=(char value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(short value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(int value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(long value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(long long value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(unsigned char value)
{	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(unsigned short value)
{	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(unsigned int value)
{	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(unsigned long value)
{	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(unsigned long long value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(float value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(double value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(long double value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	std::stringstream ss;
	ss << value;
	value_ = ss.str();
	type_ = Json::JTYPE_NUMBER;
	return *this;
}

Json& Json::operator=(bool value)
{
	if (type_ == Json::JTYPE_ARRAY || type_ == Json::JTYPE_OBJECT)
		clear();
	value_ = value ? "true" : "false";
	type_ = Json::JTYPE_BOOLEAN;
	return *this;
}

template <class T> T Json::as() const
{
	if (type_ != Json::JTYPE_STRING && type_ != Json::JTYPE_BOOLEAN && type_ != Json::JTYPE_NUMBER)
		throw "Cannot convert object or array types"; // FIXME: should throw an appropriate exception
	std::stringstream ss;
	ss << value_;
	T r;
	ss >> r;
	return r;
}

#endif // Json_H
