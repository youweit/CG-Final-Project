#ifndef CCONTROLLER_H
#define CCONTROLLER_H
#include <map>

using namespace std;

class CController
{

protected:
	std::map<int,bool> keys;
public:
	void onKeyDown(int keycode){
		keys[keycode] = true;
		//printf("Down - %c\n", keycode);
	}
	void onKeyUp(int keycode){
		keys[keycode] = false;
		//printf("Up - %c\n", keycode);
	}
	bool GetKeyState(const char* keycode){
		return keys[*keycode];
	}
	//TODO dynamic add kyes
	CController(){
		keys[97] = false;
		keys[100] = false;
		keys[115] = false;
		keys[119] = false;
		keys[32] = false;
	}
	~CController();
};

#endif