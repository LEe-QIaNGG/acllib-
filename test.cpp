#include"acllib.h"
#include"sprite.h"
#include<stdlib.h>
#include<stdio.h>
#include<ctime>

//2.音效

ACL_Image img_hunter, img_usr, img_prey,img_QR,img_DLC,img_health,img_health1;
ACL_Sound sound_bgm, sound_score, sound_crash, sound_pass, sound_fail;
const char * bgm = "bgm.mp3", * scoring = "scoring.mp3", * crashing = "crashing.mp3", * pass = "pass.mp3", * fail = "fail.mp3";
const char *sweating = "sweating.BMP",*TYT="tyt.BMP",*FaShao="fashao.BMP",*QR_code="QR.BMP",*DLC="DLC.BMP",*live="live.BMP", * live1 = "live2.BMP";
#define moveinterval  50
#define hunterinterval 4500 
#define checkinterval 100
#define timeinterval 1000
#define MAX_prey  7
#define preyinterval 1000 
int vanish = 0, num_prey = 0, num_hunter = 0;
int ifpause;//若正在暂停则ifpause为1
int deleted[20];//deleted存放被crash的prey编号,
int time2 = 0, time1 = 0,minute=1,second=30, MAX_hunter = 4;//time1调整prey生成时间,time2调整hunter生成时间
int x, y; float anglex, angley;
unsigned seed;
sprite* tyt = NULL, * Sweating[5] = { 0 }, * Fashao[MAX_prey] = { 0 };

void paintmain();
void paintend(int i);//结束画面,0时间结束失败，1生命耗尽失败，2成功,3帮助
void pause();
void timerEvent(int sid);
void KeyboardEvent(int key, int e);
void createhunter(sprite **hunter);
void createuser(sprite ** user);
void createprey(sprite** prey);
void deletesprite(int n,int i);//将已被crash的prey的编号放入栈中，后续再生成prey时使用栈中的prey
void loadsource();

int Setup() {
	initWindow("My Window", DEFAULT, DEFAULT, windowwidth, windowheight);
	loadsource();
	createuser(&tyt);
	createhunter(Sweating);
	createprey(Fashao);
	registerTimerEvent(timerEvent);
	registerKeyboardEvent(KeyboardEvent);
	playSound(sound_bgm, 1);
	startTimer(0, moveinterval);
	startTimer(1, hunterinterval);
	startTimer(2, preyinterval);
	startTimer(3, checkinterval);//检查prey存活情况,检查游戏是否结束
	startTimer(4, timeinterval);//游戏结束倒计时
	return 0;
}

void paintmain() {
	beginPaint();
	clearDevice();
	char txt1[30],txt2[20],txt3[20],txt4[30],txt5[30];
	sprintf_s(txt1, "%s","health:");
	sprintf_s(txt2, "score:%d", score);
	sprintf_s(txt3, "time:%02d:%02d", minute, second);
	sprintf_s(txt4, "%s", "拯救小猪一谈");
	sprintf_s(txt5,"%s", "按回车键查看游戏规则");
	setTextSize(30);
	setTextColor(BLUE);
	paintText(400, 10, txt4);
	setTextSize(20);
	setTextColor(RED);
	paintText(10, 10, txt1);
	putImageScale(&img_health, 120, 10, health, 20);
	putImageScale(&img_health1, 120+health, 10, 100-health, 20);
	setTextColor(GREEN);
	paintText(10, 40, txt2);
	setTextColor(BLACK);
	paintText(10, 70, txt3);
	setTextSize(15);
	paintText(800, 580, txt5);
	tyt->draw();
	for (int i = 0; i < num_hunter; i++)
		Sweating[i]->draw();
	for (int i = 0; i < num_prey; i++)
		if(Fashao[i]->ifexist())
		   Fashao[i]->draw();
	endPaint();
}
void paintend(int i) {
	char txt1[30], txt2[40],txt3[40];
	sprintf_s(txt1, "%s", "你失败了");
	sprintf_s(txt3, "%s", "---首充5元，得至尊VIP作弊码--");
	beginPaint();
	clearDevice();
	switch (i) {
	case 0:
		stopSound(sound_bgm);
		setTextSize(50);
		setTextColor(RED);
		sprintf_s(txt2, "%s", "一谈未及时消灭");
		paintText(400, 30, txt1);
		setTextSize(40);
		paintText(360, 160, txt2);
		setTextColor(BLUE);
		paintText(230, 220, txt3);
		putImageScale(&img_QR, 360, 300, 200, 250);
		putImageScale(&img_prey, 650, 160, 40, 40);
		putImageScale(&img_hunter, 600, 160, 45, 45);
		break;
	case 1:
		stopSound(sound_bgm);
		setTextSize(50);
		setTextColor(RED);
		sprintf_s(txt2, "%s", "一谈被豆控制了");
		paintText(400, 30, txt1);
		setTextSize(40);
		paintText(360, 160, txt2);
		setTextColor(BLUE);
		paintText(230, 220, txt3);
		putImageScale(&img_QR, 360, 300, 200, 250);
		putImageScale(&img_hunter, 480, 160, 45, 45);
		putImageScale(&img_hunter, 600, 30, 45, 45);
		break;
	case 2:
		stopSound(sound_bgm);
		sprintf_s(txt1, "%s", "你通关了");
		sprintf_s(txt3, "%s", "---首充5元，预购DLC--");
		setTextSize(50);
		setTextColor(RED);
		sprintf_s(txt2, "%s", "你可真是个小天才！");
		paintText(400, 30, txt1);
		setTextSize(40);
		paintText(360, 160, txt2);
		setTextColor(BLUE);
		paintText(230, 220, txt3);
		putImageScale(&img_QR, 600, 300, 200, 250);
		putImageScale(&img_DLC, 150, 300, 400, 300);
		break;
	case 3:
		char txt4[60];
		sprintf_s(txt1, "%s", "游戏说明:");
		sprintf_s(txt4, "%s", "得到100分来拯救小猪一谈,时间耗尽或生命值时游戏结束");
		sprintf_s(txt2, "%s", "碰撞—会扣生命值,碰撞—以得分");
		sprintf_s(txt3, "%s", "按esc键返回游戏");
		setTextSize(50);
		setTextColor(RED);
		paintText(10, 10, txt1);
		setTextSize(35);
		setTextColor(BLUE);
		paintText(10, 70, txt4);
		setTextSize(40);
		paintText(10, 120, txt2);
		paintText(10, 160, txt3);
		putImageScale(&img_hunter, 92, 120, 40, 40);
		putImageScale(&img_prey, 442, 120, 45, 45);
		break;
	default:
		break;
	}
	endPaint();
}
void timerEvent(int tid) {
	if(tid==0){
		for (int i = 0; i < num_hunter; i++) {
			if (Sweating[i]->crash(tyt) != 1) {
				Sweating[i]->move(0, tyt);
			}
		}
		for (int i = 0; i < num_prey; i++) {
			if (Fashao[i]->ifexist()) {
				Fashao[i]->move(0,tyt);
				Fashao[i]->crash(tyt);
			}
		}	
		paintmain();
		return;
	}
	else if (tid == 1) {//用time计数实现精灵生成速度变化
		if (minute <= 0 || score >= 40) {
			MAX_hunter = 5;
			if (score >= 70 && MAX_hunter != 7)
				MAX_hunter = 7;
		}
			MAX_hunter = 5;
		if (num_hunter < MAX_hunter && num_hunter >= 0) {
			createhunter(Sweating);
		}
		return;
	}
	else if (tid == 2) {
		if (num_prey <= MAX_prey && num_prey >= 0) {
			if (time1 == 4)
				time1 = 0;
			time1++;
			if(num_prey-vanish<MAX_prey/2)
				createprey(Fashao);
			else if(num_prey-vanish<2*MAX_prey/3 && (time1==1 || time1==3) )
				createprey(Fashao);
			else if(time1==2)
				createprey(Fashao);
		 }		
		return;
	}
	else if (tid == 3) {
		if (score >= 100) {
			playSound(sound_pass, 0);
			cancelTimer(0);
			cancelTimer(1);
			cancelTimer(2);
			cancelTimer(3);
			cancelTimer(4);
			paintend(2);
		}
		else if (health <= 0) {
			playSound(sound_fail, 0);
			cancelTimer(0);
			cancelTimer(1);
			cancelTimer(2);
			cancelTimer(3);
			cancelTimer(4);
			paintend(1);
		}
		else if (minute <= -1) {
			playSound(sound_fail, 0);
			cancelTimer(0);
			cancelTimer(1);
			cancelTimer(2);
			cancelTimer(3);
			cancelTimer(4);
			paintend(0);
		}
		for (int i = 0; i < num_prey; i++) {
			if(Fashao[i]->getchecked())
			   deletesprite(Fashao[i]->ifexist(), i);
		}
		return;
	}
	else if (tid == 4) {
        if (second != 0)
			second--;
		else {
			minute--;
			second = 60;
		}
	}
}
void deletesprite(int n, int i) {
	if (n == 1) return;
		deleted[vanish] = i;
		vanish++;
		Fashao[i]->check();//将checked置零
}
void pause() {
	cancelTimer(0);
	cancelTimer(1);
	cancelTimer(2);
	cancelTimer(3);
	cancelTimer(4);
	paintend(3);
}
void KeyboardEvent(int key, int e) {
	if (e !=KEY_DOWN || minute <= -1 || health <= 0 || score >= 100) return;
	else if (key == VK_ESCAPE && ifpause==1) {
		ifpause = 0;
		startTimer(0, moveinterval);
		startTimer(1, hunterinterval);
		startTimer(2, preyinterval);
		startTimer(3, checkinterval);
		startTimer(4, timeinterval);
	}
	else if (key == VK_RETURN && ifpause!=1) {
		ifpause = 1;
		pause();
	}
	if (ifpause) return;
		tyt->move(key,tyt);
		paintmain();

}
void createuser(sprite** user) {
	int width=70, height=100;
	seed = time(0);
	srand(seed);
	x = rand() % (windowwidth - width + 1);
	y = rand() % (windowheight - height + 1);
	anglex = (rand() % 10) / 10.0;
	angley = (rand() % 10) / 10.0;
	*user = new usrsprite(x,y,anglex,angley,width,height,&img_usr,&sound_bgm);
}
void createhunter(sprite** hunter) {
	int width = 50, height = 50;
	seed = time(0);
	srand(seed);
	x = rand() % (windowwidth - width + 1);
	y = rand() % (windowheight - height + 1);
	anglex = (rand() % 100) / 100.0;
	angley = 1-anglex*anglex;
	hunter[num_hunter++] = new huntersprite(x, y, anglex, angley, width, height,&img_hunter,&sound_crash);
}
void createprey(sprite** prey) {
	int width = 40, height = 40;	
	if (vanish>0) {//使用已被crash掉的prey来生成
		vanish--;
		Fashao[deleted[vanish]]->unvanish();
		return;
	}
	seed = time(0);
	srand(seed);
	x = rand() % (windowwidth - width + 1);
	y = rand() % (windowheight - height + 1);
	anglex = (rand() % 100) / 100.0;
	angley = 1-anglex*anglex;
	prey[num_prey++] = new preysprite(x, y, anglex, angley, width, height, &img_prey,&sound_score);
}
void loadsource() {
	loadImage(sweating, &img_hunter);
	loadImage(TYT, &img_usr);
	loadImage(FaShao, &img_prey);
	loadImage(QR_code, &img_QR);
	loadImage(DLC, &img_DLC);
	loadImage(live, &img_health);
	loadImage(live1, &img_health1);
	loadSound("bgm.mp3", &sound_bgm);
	loadSound(scoring, &sound_score);
	loadSound(crashing, &sound_crash);
	loadSound(pass, &sound_pass);
	loadSound(fail, &sound_fail);
}