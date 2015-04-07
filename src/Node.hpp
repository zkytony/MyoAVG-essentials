#ifndef NODE_H
#define NODE_H

class Node
{
public:
	enum TYPE {
		START, END, NORMAL, NOTHING, CONFLICT, TRESURE
	};
	TYPE type = TYPE::NOTHING; 
	
	bool has_come = false; // you have come here before
	std::string msg = "Nothing bro";
	int x; // x coordinate
	int y; // y coordinate
};

#endif