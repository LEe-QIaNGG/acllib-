#pragma once
#include<math.h>
#define windowwidth  1000
#define windowheight  600
int score=0,health=100;

//基类
class preysprite;
class sprite {
	friend class preysprite;
	friend class huntersprite;
protected:
	bool exist,ifcrash;//exist为0不显示，为1显示；ifcrash为1时正在碰撞，为0时没有
	int checked;//若已进入删除栈checked为0
	int x, y,px,py;
	int width, height;
	float anglex, angley;
	ACL_Image *img;	
	ACL_Sound *sound;
public:
	sprite() {}
	sprite(int x, int y, float anglex,float angley,ACL_Image *img, ACL_Sound* sound) {
		this->x = x;
		this->y = y;
		this->anglex = anglex;
		this->angley = angley;
		this->img = img;
		this->sound = sound;
		this->exist = 1;
		this->ifcrash = 0;
		this->checked = 1;
	}
	virtual void move(int key,sprite *user){}
	virtual int crash(sprite* user) { return 0; };//发生碰撞则返回1
	virtual void check() {  }
	void draw() {
		putImageTransparent(img, x, y, width, height, WHITE);
	}
	void unvanish() { 
		this->exist = 1; 
		this->checked = 1;
	}
	int ifexist() { return exist; }
	int getpx() { return px; }
	int getpy() { return py; }	
	int getchecked() { return this->checked; }//若已进入删除栈，则返回0
};
//子类
class usrsprite :public sprite {	
	int dist = 10;
public:
	usrsprite(){}
	usrsprite(int x, int y, float anglex, float angley,int width,int height, ACL_Image *img, ACL_Sound* sound):sprite(x,y,anglex,angley,img,sound) {
		this->width = width;
		this->height = height;
		px = int(x + width * 0.5);
		py = int(y + height * 0.5);
	}
	void move(int key, sprite* user) {
		switch (key)
		{
		case(VK_UP):
			 y =  y - dist;
			if ( y < 0)  y = 0;
			break;
		case(VK_DOWN):
			 y =  y + dist;
			if ( y > windowheight -  height)  y = windowheight -  height;
			break;
		case(VK_RIGHT):
			 x =  x + dist;
			if ( x > windowwidth -  width)  x = windowwidth -  width;
			break;
		case(VK_LEFT):
			 x =  x - dist;
			if ( x < 0)  x = 0;
			break;
		default:
			break;
		}
		px = int(x + width * 0.5);
		py = int(y + height * 0.5);
	}
};

class preysprite :public sprite {
	int distance1 = 7,distance2=7,escapedist=60;
public:
	preysprite(){}
	preysprite(int x, int y, float anglex, float angley, int width, int height, ACL_Image *img, ACL_Sound* sound):sprite(x, y, anglex, angley, img,sound) {
		this->width = width;
		this->height = height;
		px = int(x + width * 0.5);
		py = int(y + height * 0.5);
	}
	void move(int key, sprite* user) {
		if (score >= 60) {
			escapedist = 120;
		}	
		if (abs(user->px - this->px) < escapedist && abs(user->py - this->py) < escapedist) {
			float r2,r;
			distance1 = -abs(distance1);
			distance2 = -abs(distance2);
			r2 = (user->px*1.0 - this->px*1.0) * (user->px*1.0 - this->px) + (user->py*1.0 - this->py) * (user->py*1.0 - this->py);
			r = sqrt(r2);
			anglex = (user->px - this->px) / r;
			angley = (user->py - this->py) / r;
		}
		if ( x > windowwidth -  width ||  x < 0)
			distance1 = -1 * distance1;
		 x = (int)( x + anglex * distance1);
		if ( y > windowheight -  height ||  y < 0)
			distance2 = -1 * distance2;
		 y =(int)(  y + angley * distance2);
		 px = int(x + width * 0.5);
		 py = int(y + height * 0.5);
	}
	int crash(sprite* user) {
		if (this->exist == 0) return 0;
		if (abs(this->px - user->px) * 2 < this->width + user->width && abs(this->py - user->py) * 2 < this->height + user->height) {
			this->exist = 0;
		    score = score + 5;
			playSound(*sound, 0);
			return 1;
		}
		return 0;
	}
	void check() {
		this->checked = 0;
	}
};

class huntersprite :public sprite {
	int distance1 = 8, distance2 = 8,chasedist=80;
	bool  away;
public:

	huntersprite(int x, int y, float anglex, float angley, int width, int height, ACL_Image *img, ACL_Sound* sound) :sprite(x, y, anglex, angley, img,sound) {
		this->width = width;
		this->height = height;
		px = int(x + width * 0.5);
		py = int(y + height * 0.5);
		this->away = 1;
	}
	void move(int key, sprite* user) {
		if (score >= 80)
			chasedist = 140;
		if (abs(user->px - this->px) < chasedist && abs(user->py - this->py) < chasedist) {
			float r2,r;
			distance1 = 8;
			distance2 = 8;
			r2 = (user->px*1.0 - this->px) * (user->px*1.0 - this->px)/1.0 + (user->py*1.0 - this->py) * (user->py*1.0 - this->py);
			r = sqrt(r2);
			anglex = (user->px - this->px) / r;
			angley = (user->py - this->py) / r;
		}
		if ( x > windowwidth -  width ||  x < 0)
			distance1 = -1 * distance1;
		 x = (int)( x + anglex * distance1);
		if ( y > windowheight -  height ||  y < 0)
			distance2 = -1 * distance2;
		 y = (int)( y + angley * distance2);	
		 px = int(x + width * 0.5);
		 py = int(y + height * 0.5);
	}
	int crash(sprite* user) {
		if (abs(this->px - user->px) * 2 < (this->width + user->width) && abs(this->py - user->py) * 2 < (this->height + user->height)) {
			this->ifcrash = 1;
			if (this->ifcrash == 1 && this->away == 1) {
				health = health - 20;
				playSound(*sound, 0);
				this->away = 0;
			}
			return 1;
		}
		else if (abs(this->px - user->px) * 2 >= (this->width + user->width) && abs(this->py - user->py) * 2 >= (this->height + user->height)) {
			this->ifcrash = 0;
			this->away = 1;
			return 0;
		}
		return 0;
	}
};
