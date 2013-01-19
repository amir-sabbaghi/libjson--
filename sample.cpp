#include <iostream>
#include "json/json.h"

using namespace std;

int main()
{
	// reading a json file
	Json input("sample.json");
	
	// accessing object members
	cout << "name: " << input["name"].as<string>() << endl;
	cout << "description: " << input["description"].as<string>() << endl;
	cout << "integer member: " << input["integer"].as<int>() << endl;
	cout << "float member: " << input["float"].as<float>() << endl; // you should know the type of a member
	if (input["null"].getType() == Json::JTYPE_NULL)                // or you can examine it like this.
		cout << "there is also a null member" << endl;
	
	// iterating
	cout << "array: ";
	for (Json::ArrayIterator i = input["array"].arrayBegin(); i != input["array"].arrayEnd(); i ++)
		cout << (*i)->as<int>() << ',';
	cout << endl;
	
	// creating a json
	Json output;
	output.setType(Json::JTYPE_OBJECT);
	output["hello"] = "world";
	output["number"] = 1;             // note: 1 will be ambiguous, use 1.0 or 1L or 1.0f or ...
	for (int i = 0; i < 10; i ++)
		output["array"][i] = i + 1;
	output["object"]["bool"] = true;
	output["object"]["null"].setType(Json::JTYPE_NULL);
	
	// writing to a file
	output.save("output.json");
	
	return 0;
}
