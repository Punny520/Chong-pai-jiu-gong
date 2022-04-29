#include <graphics.h>
#include<ege/sys_edit.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#define DIF_easy 25
#define DIF_hard 300
int x_move[5]={0,0,0,-1,1};//向上下左右
int y_move[5]={0,-1,1,0,0};//向上下左右
int loc_x[5]={0,-1,1,0,0};//方位上下左右
int loc_y[5]={0,0,0,-1,1};//方位上下左右
int img_position_x[4]={0,20,148,276};
int img_position_y[4]={0,18+150,146+150,274+150};
int game_map[4][4];
int restart_map[4][4];
int all_solutions[17][4][4];
int IS_MOVE,leve;
int tal_move;
double start_time;
double run_time,cur_time;
int GAME_DIF;
int END_GAME=0;
mouse_msg m_inf;
struct ABMOVE{
	int tag_i=0,tag_j=0;
	int cur_x=0,cur_y=0;
	int cur_i=0,cur_j=0;
	int dir=0;
}abmove;
struct RANK{
	double final_time;
	int final_move;
	char name[50];
}rank[8];
PIMAGE img_package[9];
PIMAGE blockimg[11];
void draw_answer();
void load_img();
void load_puzzle();
void draw();
void initialize();
void load_game_data();
void swap(int &x,int &y);
void start();
void tryto_move(int x,int y);
void move_block();
void win();
void menu(int img_num);
void ChangeRank(int p);
int seledif();
void play_game();
void draw_gameinf();
void load_rank_data();
void upload_rank();
using namespace std;
int main(){
	//printf("main函数\n");
	load_img();//载入图片
	load_game_data();
	load_rank_data();
	menu(1);//绘制菜单，背景板以及其判定
	return 0;
}
void upload_rank(){
	//printf("更新排行榜\n");
	freopen("game_data\\Rank_Data.txt","w",stdout);
	for(int i=1;i<=5;i++){
		printf("%s\n",rank[i].name);
		printf("%.2lf\n",rank[i].final_time);
		printf("%d\n",rank[i].final_move);
	}
	fclose(stdout);
	//printf("更新完成\n");
	return ;
}
void load_rank_data(){
	//printf("加载排行榜\n");
	freopen("game_data\\Rank_Data.txt","r",stdin);
	for(int i=1;i<=5;i++){
		scanf("%s",rank[i].name);
		scanf("%lf",&rank[i].final_time);
		scanf("%d",&rank[i].final_move);
	}
	fclose(stdin);
}
void load_game_data(){
	//printf("加载游戏数据\n");
	char c;
	freopen("game_data\\game_data.txt","r",stdin);
	for(int i=0;i<16;i++){
		for(int j=0;j<4;j++){
			for(int k=0;k<4;k++){
				scanf("%c",&c);
				all_solutions[i][j][k]=(int)(c-'A');
			}
		}
	}
	fclose(stdin);
	//printf("load_game_data OK!\n");
}
void load_img(){//加载窗口、背景图片，以及数字方块图片
	//printf("加载图片\n");
	setcaption("重排九宫");
	initgraph(400,550);
	img_package[0]=newimage();//菜单
	img_package[1]=newimage();//背景
	img_package[2]=newimage();//选择难度
	img_package[3]=newimage();//游戏中
	img_package[4]=newimage();//排行榜
	img_package[5]=newimage();//关于
	img_package[6]=newimage();//完成
	img_package[7]=newimage();//新记录
	img_package[8]=newimage();//输入名称
	getimage(img_package[0],"game_resource\\menu.PNG");
	getimage(img_package[1],"game_resource\\background.PNG");
	getimage(img_package[2],"game_resource\\seledif.PNG");
	getimage(img_package[3],"game_resource\\playing.PNG");
	getimage(img_package[4],"game_resource\\rank.PNG");
	getimage(img_package[5],"game_resource\\about.PNG");
	getimage(img_package[6],"game_resource\\win.PNG");
	getimage(img_package[7],"game_resource\\New_record.PNG");
	getimage(img_package[8],"game_resource\\name_in.PNG");
	char block_name[50];
	for(int i=1;i<=10;i++){
		blockimg[i]=newimage();
		sprintf(block_name,"game_resource\\block_%d.PNG",i);
		getimage(blockimg[i],block_name);
	}
	//printf("load_img OK!\n");
}
void initialize(){//初始化
	//printf("初始化\n");
	tal_move=0;
	leve=0;
	IS_MOVE=0;
	END_GAME=0;
	GAME_DIF=0;
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			restart_map[i][j]=game_map[i][j]=0;
		}
	}
	//printf("initalize OK!\n");
}
void draw_block(){
	//printf("画方块\n");
	int ck=1;
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			if(game_map[i][j]!=all_solutions[leve][i][j]) ck=0;
			if(!game_map[i][j]) continue;
			if(IS_MOVE&&i==abmove.cur_i&&j==abmove.cur_j){
				if(abmove.cur_x==img_position_x[abmove.tag_j]&&abmove.cur_y==img_position_y[abmove.tag_i]){
					IS_MOVE=0;
					putimage(abmove.cur_x,abmove.cur_y,blockimg[game_map[i][j]]);		
					swap(game_map[abmove.tag_i][abmove.tag_j],game_map[i][j]);
					abmove={0,0,0,0,0,0,0};
				}else{
					abmove.cur_x+=16*x_move[abmove.dir];
					abmove.cur_y+=16*y_move[abmove.dir];
					putimage(abmove.cur_x,abmove.cur_y,blockimg[game_map[i][j]]);
				}
				
			}else putimage(img_position_x[j],img_position_y[i],blockimg[game_map[i][j]]);
		}
	}
	if(ck) win();
}
void win(){
	//printf("赢了\n");
	if(GAME_DIF==DIF_hard){
		for(int i=1;i<=5;i++){
			if((cur_time<rank[i].final_time)||(cur_time==rank[i].final_time&&tal_move<rank[i].final_move)){
				ChangeRank(i);
			}
		}	
	}
	setfillcolor(EGEACOLOR(0x60, WHITE));
	ege_fillrect(0,150,400,550);
	putimage_withalpha(NULL,img_package[6],160,330);
	Sleep(2000);
	menu(1);
}
void ChangeRank(int p){//更新记录
	//printf("输入姓名\n");
	char Your_Name[50];
	setfillcolor(EGEACOLOR(0x60, WHITE));
	ege_fillrect(0,150,400,550);
	putimage_withalpha(NULL,img_package[7],120,330);
	putimage(0,0,img_package[8]);
	sys_edit editBox;
	editBox.create(false);
	editBox.setmaxlen(20);//最大输入20个字符
	editBox.setcolor(EGERGB(189,170,157));
	editBox.setfont(30,0,"宋体");
	editBox.setbgcolor(EGERGB(70,70,70));
	editBox.move(84,89);
	editBox.size(191,48);
	editBox.visible(true);
	editBox.setfocus();
	Sleep(2000);
	putimage(0,150,img_package[1]);
	putimage(0,0,img_package[8]);
	for(;is_run();delay_fps(60)){
		m_inf=mouse_msg();
		while(mousemsg()){
			m_inf=getmouse();
		}
		if(m_inf.x>=279&&m_inf.x<=354&&m_inf.y>=88&&m_inf.y<=137&&m_inf.is_left()&&m_inf.is_down()){
			editBox.gettext(45,Your_Name);
			editBox.destroy();
			break;
		}
	}
	flushkey();
	for(int i=5;i>p;i--){
		rank[i].final_move=rank[i-1].final_move;
		rank[i].final_time=rank[i-1].final_time;
		strcpy(rank[i].name,rank[i-1].name);
	}
	rank[p].final_move=tal_move;
	rank[p].final_time=cur_time;
	strcpy(rank[p].name,Your_Name);
	upload_rank();
	menu(1);
}
int seledif(){
	//printf("选择难度\n");
	putimage(0,0,img_package[2]);
	putimage(0,150,img_package[1]);
	for(;is_run();delay_fps(60)){
		m_inf=mouse_msg();
		while(mousemsg()){
			m_inf=getmouse();
		}
		if(m_inf.x>=84&&m_inf.x<=198&&m_inf.y>=12&&m_inf.y<=66&&m_inf.is_left()&&m_inf.is_down()) return DIF_easy;
		if(m_inf.x>=84&&m_inf.x<=198&&m_inf.y>=81&&m_inf.y<=138&&m_inf.is_left()&&m_inf.is_down()) return DIF_hard;
	}
	return 0;
}
void start(){
	//printf("开始\n");
	//printf("Now start!\n");
	GAME_DIF=seledif();
	//printf("GAME_DIF is %d\n",GAME_DIF);
	load_puzzle();
	play_game();
}
void load_puzzle(){
	//printf("加载题目\n");
	int P0_x=0,P0_y=0,TO;
	if(!leve){//第一次加载游戏
		//printf("first load puzzle!\n");
		srand(time(NULL));
		leve=16-(rand()%16);//随机选择一个解
		//printf("the leve is %d\n",leve);
		for(int i=1;i<=3;i++){
			for(int j=1;j<=3;j++){
				game_map[i][j]=all_solutions[leve][i][j];
				if(!game_map[i][j]){//找到空格
					P0_x=i;
					P0_y=j;
				}
			}
		}
		for(int i=1;i<=GAME_DIF;i++){//根据解逆序随机打乱保证题目有解
			TO=4-rand()%4;
			if(P0_x+loc_x[TO]>3||P0_x+loc_x[TO]<1) continue;
			if(P0_y+loc_y[TO]>3||P0_y+loc_y[TO]<1) continue;
			swap(game_map[P0_x+loc_x[TO]][P0_y+loc_y[TO]],game_map[P0_x][P0_y]);
			P0_x+=loc_x[TO];
			P0_y+=loc_y[TO];
		}
		for(int i=1;i<=3;i++){//记录初始打乱状态
			for(int j=1;j<=3;j++){
				restart_map[i][j]=game_map[i][j];
			}
		}
		return ;
	}else{
		//printf("restart\n");
		for(int i=1;i<=3;i++){//还原初始打乱状态
			for(int j=1;j<=3;j++){
				game_map[i][j]=restart_map[i][j];
			}
		}
		play_game();
	}
}
void game_pause(){
	//printf("游戏暂停\n");
	if((int)(getch())==0x20) return ;
	else game_pause();
}
void play_game(){
	//printf("玩游戏\n");
	for(;is_run();delay_fps(60)){
		bool Click1=false;
		bool Click2=false;
		bool Click3=false;
		bool Click4=false;
		if(!tal_move) 	start_time=fclock();
		draw_gameinf();
		m_inf=mouse_msg();
		while(mousemsg()){
			m_inf=getmouse();
			if(m_inf.x>=84&&m_inf.x<=198&&m_inf.y>=54&&m_inf.y<=90&&m_inf.is_left()&&m_inf.is_down()) Click1=true;
			if(!IS_MOVE&&m_inf.is_left()&&m_inf.is_down()) Click2=true;
			if(m_inf.x>=84&&m_inf.x<=198&&m_inf.y>=100&&m_inf.y<=135&&m_inf.is_left()&&m_inf.is_down()) Click3=true;
			if(m_inf.x>=249&&m_inf.x<=388&&m_inf.y>=107&&m_inf.y<=139&&m_inf.is_left()&&m_inf.is_down()) Click4=true;
		}
		if(Click1){//重新开始
			tal_move=0;
			load_puzzle();
		}
		draw_block();
		while(kbhit()){
			int ch=getch();
			if(ch==0x20) game_pause();
			else break;
		}
		if(Click2) tryto_move(m_inf.x,m_inf.y);
		if(Click3) menu(1);
		if(Click4) draw_answer();
	}
}
void draw_answer(){
	//printf("看答案\n");
	putimage(0,150,img_package[1]);
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			if(!all_solutions[leve][i][j]) continue;
			putimage(img_position_x[j],img_position_y[i],blockimg[all_solutions[leve][i][j]]);
		}
	}
	Sleep(3500);
	start_time+=3.5;
	return ;
}
void draw_gameinf(){
	//printf("画游戏信息\n");
	putimage(0,0,img_package[3]);
	putimage(0,150,img_package[1]);
	cur_time=fclock();
	run_time=cur_time-start_time;
	setcolor(EGERGB(70,70,70));
	setfont(25,0,"楷体");
	setbkmode(TRANSPARENT);
	xyprintf(306,15,"%.2lf",run_time);
	xyprintf(306,70,"%d",tal_move);
}
void swap(int &x,int &y){
	int tmp=y;
	y=x;
	x=tmp;
}
void tryto_move(int x,int y){
	//printf("移动\n");
	for(int i=1;i<=3;i++){
		for(int j=1;j<=3;j++){
			if(game_map[i][j]!=0&&img_position_x[j]<x&&img_position_y[i]<y&&x<=img_position_x[j]+109&&y<=img_position_y[i]+109){
				for(int k=1;k<=4;k++){
					if(i+loc_x[k]>3||i+loc_x[k]<1) continue;
					if(j+loc_y[k]>3||j+loc_y[k]<1) continue;
					if(game_map[i+loc_x[k]][j+loc_y[k]]==0){
						abmove.tag_i=i+loc_x[k];
						abmove.tag_j=j+loc_y[k];
						abmove.cur_x=img_position_x[j];
						abmove.cur_y=img_position_y[i];
						abmove.cur_i=i;
						abmove.cur_j=j;
						abmove.dir=k;
						IS_MOVE=1;
						tal_move++;
						return ;
					}
				}
			}
		}
	}
	return ;
}
void menu(int img_num){
	//printf("在菜单\n");
	//printf("Now in meun\n");
	putimage(0,0,img_package[0]);
	putimage(0,150,img_package[img_num]);
	if(img_num==4){
		setcolor(EGERGB(193,169,155));
		setfont(20,0,"楷体");
		setbkmode(TRANSPARENT);
		for(int i=1;i<=5;i++){
			xyprintf(100,210+i*50,"%s %.2lf秒 %d步",rank[i].name,rank[i].final_time,rank[i].final_move);
		}
	}
	for(;is_run();delay_fps(60)){
		m_inf=mouse_msg();
		while(mousemsg()){
			m_inf=getmouse();
			if(m_inf.is_left()&&m_inf.is_down()&&m_inf.x>=82&&m_inf.x<=200&&m_inf.y>=3&&m_inf.y<=33){
				initialize();//开始游戏前初始化
				start();//开始游戏
			}
			if(m_inf.is_left()&&m_inf.is_down()&&m_inf.x>=82&&m_inf.x<=200&&m_inf.y>=42&&m_inf.y<=76){
				menu(4);//排行榜
			}
			if(m_inf.is_left()&&m_inf.is_down()&&m_inf.x>=82&&m_inf.x<=200&&m_inf.y>=84&&m_inf.y<=113){
				menu(5);//关于
			}
			if(m_inf.is_left()&&m_inf.is_down()&&m_inf.x>=82&&m_inf.x<=200&&m_inf.y>=124&&m_inf.y<=146){
				closegraph();
				exit(0);
			}
		}
	}
}
