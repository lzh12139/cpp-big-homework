#include "stdafx.h"
#include "game.h"

const int dd_left[3] = { 1,-1,0 };
const int dd_up[3][2] = { 2,0,1,-1,1,1 };
const int dd[4][2] = { 0,-1,1,0,0,0,-1,0 };
const int speed = 1;
const int person_r = 3;
const int wall_r = 1;
/*char sss[10];
wchar_t s[10];
sprintf_s(sss, sizeof(sss), "%d", pos.ff);
change(sss, s);
outtextxy(0, 0, s);

sprintf_s(sss, sizeof(sss), "%d", pos.ss);
change(sss, s);
outtextxy(0, 100, s);
*/

void change(char *ss, wchar_t *s) {
	int i;
	for (i = 0; ss[i]; i++)
		s[i] = ss[i];
	s[i] = '\0';
}

int dis(int x1, int y1, int x2, int y2) {
	return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
}

// Command
void Command::get_kb(const int do_type, const int pause_type) {
	hit = false;
	kb = 0;

	if (do_type == 1) {
		if (GetAsyncKeyState('A')) kb |= 1;
		if (GetAsyncKeyState('D')) kb |= 2;
		if (GetAsyncKeyState('W')) kb |= 4;
		if (GetAsyncKeyState('S')) kb |= 8;
	}
	else {
		if (GetAsyncKeyState(VK_LEFT)) kb |= 1;
		if (GetAsyncKeyState(VK_RIGHT)) kb |= 2;
		if (GetAsyncKeyState(VK_UP)) kb |= 4;
		if (GetAsyncKeyState(VK_DOWN)) kb |= 8;
	}

	if (pause_type == 1 && GetAsyncKeyState('P'))
		kb |= 16;//5
	else if (pause_type == 2 && GetAsyncKeyState(VK_ESCAPE))
		kb |= 16;//6 ESC

	MOUSEMSG mm;
	if (MouseHit()) {
		mm = GetMouseMsg();
		if (mm.mkLButton)
			hit = true;
		pos.ff = mm.x, pos.ss = mm.y;
	}
}

// Map
Map::Map() {
	ok = false;

	loadimage(&p0000, L"image\\0000.jpg", 10, 10, true);
	loadimage(&p0001, L"image\\0001.jpg", 10, 10, true);
	loadimage(&p0010, L"image\\0010.jpg", 10, 10, true);
	loadimage(&p0011, L"image\\0011.jpg", 10, 10, true);
	loadimage(&p0100, L"image\\0100.jpg", 10, 10, true);
	loadimage(&p0101, L"image\\0101.jpg", 10, 10, true);
	loadimage(&p0110, L"image\\0110.jpg", 10, 10, true);
	loadimage(&p0111, L"image\\0111.jpg", 10, 10, true);
	loadimage(&p1000, L"image\\1000.jpg", 10, 10, true);
	loadimage(&p1001, L"image\\1001.jpg", 10, 10, true);
	loadimage(&p1010, L"image\\1010.jpg", 10, 10, true);
	loadimage(&p1011, L"image\\1011.jpg", 10, 10, true);
	loadimage(&p1100, L"image\\1100.jpg", 10, 10, true);
	loadimage(&p1101, L"image\\1101.jpg", 10, 10, true);
	loadimage(&p1110, L"image\\1110.jpg", 10, 10, true);
	loadimage(&p1111, L"image\\1111.jpg", 10, 10, true);

	loadimage(&qizi_, L"image\\qizi.jpg", 120, 60, true);
	SetWorkingImage(&qizi_);
	for (int i = 0; i < 4; i++) {
		getimage(&qizi[i][0], i * 30, 0, 30, 30);
		getimage(&qizi[i][1], i * 30, 30, 30, 30);
	}

	SetWorkingImage(NULL);

	loadimage(&time_, L"image\\time_.jpg");
}

Map::~Map() {

}

void Map::vis_edge() {
	for (int i = 0; i <= width + 1; i++) {
		vis[0][i]++;
		vis[2 * height][i]++;
		edge[0][i]++;
		edge[height + 1][i]++;
	}
	for (int i = 1; i <= height; i++) {
		vis[i][0]++;
		vis[i][width]++;
		edge[i][0]++;
		edge[i][width + 1]++;
	}
}

void Map::vis_circle() {
	int r = width / 2;
	for (int i = 0; i <= r; i++)
		for (int o = 0; o <= r; o++) {
			if (dis((r - i) * 20 + 10, (r - o) * 20 + 10, 0, 0) - r*r * 20 * 20 > 0) {
				for (int p = 0; p < 4; p++) {
					vis[i * 2 - 1 + dd[p][0]][o + dd[p][1]]++;
					vis[i * 2 - 1 + dd[p][0]][2 * r - o + 1 + dd[p][1]]++;
					vis[4 * r - 2 * i + dd[p][0]][o + dd[p][1]]++;
					vis[4 * r - 2 * i + dd[p][0]][2 * r - o + 1 + dd[p][1]]++;
				}

				ans[i][o]++;
			}
		}

	// left
	for (int i = 0; i <= r; i++)
		for (int o = r; o >= 0; o--)
			if (ans[i][o]) {
				edge[i][o]++;
				edge[width - i + 1][o]++;
				edge[width - i + 1][width - o + 1]++;
				edge[i][width - o + 1]++;
				break;
			}

	// up
	for (int o = r; o >= 0; o--)
		for (int i = r; i >= 0; i--)
			if (ans[i][o]) {
				edge[i][o]++;
				edge[width - i + 1][o]++;
				edge[width - i + 1][width - o + 1]++;
				edge[i][width - o + 1]++;
				break;
			}

	memset(ans, 0, sizeof ans);
}

void Map::vis_heart() {
	int r = (width - 1) / 2;
	int cy = height * 10 + 360;

	for (int i = 1; i <= r; i++) {
		int y2 = int((sqrt(r*r - i*i) + pow(i, (double)2 / 3) + r) * 20 + 0.5);
		int y1 = int((-sqrt(r*r - i*i) + pow(i, (double)2 / 3) + r) * 20 + 0.5);

		for (int o = 1; o <= height; o++) {
			int y = (height - o) * 20 + 10;
			if (y<y1 || y>y2) {

				for (int p = 0; p < 4; p++) {
					vis[2 * o - 1 + dd[p][0]][r + i + 1 + dd[p][1]]++;
					vis[2 * o - 1 + dd[p][0]][r - i + 1 + dd[p][1]]++;
					ans[o][r + i + 1]++, ans[o][r - i + 1]++;
				}
			}
		}
	}

	for (int i = 1; i < r / 2; i++)
		for (int p = 0; p < 4; p++) {
			vis[i * 2 - 1 + dd[p][0]][r + 1 + dd[p][1]]++;
			ans[i][r + 1]++;
		}

	/*for (int i = 0; i <= height + 1; i++) {
		for (int o = 0; o <= width + 1; o++) {
			if (!ans[i][o]) {
				edge[i][o - 1]++;
				edge[i][width - o + 2]++;
				break;
			}
		}
	}

	for (int i = 0; i <= width + 1; i++)
		for (int o = 0; o <= height + 1; o++)
			if (!ans[o][i]) {
				edge[o - 1][i]++;
				//break;
			}

	for (int i = 0; i <= width + 1; i++)
		for (int o = height + 1; o >= 0; o--)
			if (!ans[o][i]) {
				edge[o + 1][i]++;
				//break;
			}

			*/
	memset(ans, 0, sizeof ans);
}

void Map::make_new_map() {
	while (1) {
		vector< pair<int, int> >list;
		memset(a, 0, sizeof a);
		memset(ans, 0, sizeof ans);
		memset(vis, 0, sizeof vis);
		memset(edge, 0, sizeof edge);
		memset(b, 0, sizeof b);

		switch (type) {
		case 1:
			vis_edge();
			break;
		case 2:
			vis_circle();
			break;
		case 3:
			vis_heart();
		}

		//  生成起点
		int cmp = 0;
		int randx = rand() % height + 1, randy = rand() % width + 1;
		for (int p = 0; p < 4; p++) {
			int xx = 2 * randx - 1 + dd[p][0];
			int yy = randy + dd[p][0];
			if (check(xx, yy) && !vis[xx][yy]) {
				list.push_back(pa(xx, yy));
				vis[xx][yy]++;
				cmp++;
			}
		}

		if (cmp)
			a[randx][randy]++;
		else
			continue;

		// 迷宫生成
		while (!list.empty()) {
			int rx = rand() % list.size();
			pair<int, int> x = list[rx];
			vis[x.ff][x.ss]++;
			list.erase(list.begin() + rx);

			if (x.ff % 2) {
				if (a[x.ff / 2 + 1][x.ss] ^ a[x.ff / 2 + 1][x.ss + 1]) {
					b[x.ff][x.ss]++;
					if (a[x.ff / 2 + 1][x.ss]) {
						// right
						a[x.ff / 2 + 1][x.ss + 1] = 1;
						for (int i = 0; i < 3; i++) {
							int xx = x.ff + dd_left[i], yy = x.ss + 1;
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;
						}
					}
					else {
						// left
						a[x.ff / 2 + 1][x.ss] = 1;
						for (int i = 0; i < 3; i++) {
							int xx = x.ff + dd_left[i], yy = x.ss - 1;
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;
						}
					}
				}
			}
			else {
				if (a[x.ff / 2][x.ss] ^ a[x.ff / 2 + 1][x.ss]) {
					b[x.ff][x.ss]++;
					if (a[x.ff / 2][x.ss]) {
						// down
						a[x.ff / 2 + 1][x.ss] = 1;
						for (int i = 0; i < 4; i++) {
							int xx = x.ff + dd_up[i][0], yy = x.ss + dd_up[i][1];
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;
						}
					}
					else {
						// up
						a[x.ff / 2][x.ss] = 1;
						for (int i = 0; i < 4; i++) {
							int xx = x.ff - dd_up[i][0], yy = x.ss + dd_up[i][1];
							if (check(xx, yy) && !vis[xx][yy])
								list.push_back(pa(xx, yy)), vis[xx][yy]++;
						}
					}
				}
			}
		}

		// bfs找是否有通路
		queue< pair<int, int> >q;
		int diss[100][100], pre[100][100], root = 2, find = 0;
		min_step = 0;

		int rect[2][4] = { 1,1,height,width,height / 4,1,height * 3 / 4,width };
		//int he[][] = {};

		int ra, xx;

		switch (type) {
		case 1:
			ra = rand() % 2;

			switch (ra) {
			case 0:
				startx = rect[0][0], starty = rect[0][1];
				endx = rect[0][2], endy = rect[0][3];
				break;
			case 1:
				startx = rect[1][0], starty = rect[1][1];
				endx = rect[1][2], endy = rect[1][3];
				break;
			}
			break;
		case 2:
			ra = rand() % 4;
			ra = 1;
			switch (ra) {
			case 0:
				xx = int(height*sqrt(2) / 4.0 + 0.5);
				for (int i = 0; i <= height; i++)
					if (edge[xx][i] && !edge[xx][i + 1]) {
						startx = xx, starty = i + 1;
						endx = height - xx + 1, endy = height - i;
						break;
					}
				break;
			case 1:
				xx = int(height*sqrt(2) / 4.0 + 0.5);
				for (int i = height; i >= 0; i--)
					if (!edge[xx][i] && edge[xx][i + 1]) {
						startx = xx, starty = i;
						endx = height - xx + 1, endy = height - i + 1;
						break;
					}
				break;
			case 2:
				xx = int(height*sqrt(2) / 4.0 + 0.5) + height / 2;
				for (int i = 0; i <= height; i++)
					if (edge[xx][i] && !edge[xx][i + 1]) {
						startx = xx, starty = i + 1;
						endx = height - xx + 1, endy = height - i;
						break;
					}
				break;
			case 3:
				xx = int(height*sqrt(2) / 4.0 + 0.5) + height / 2;
				for (int i = height; i >= 0; i--)
					if (!edge[xx][i] && edge[xx][i + 1]) {
						startx = xx, starty = i;
						endx = height - xx + 1, endy = height - i + 1;
						break;
					}
			}
			break;
		case 3:
			int r = (width - 1) / 2;
			startx = r/2, starty = r/2, endx = height-1, endy = r+1;
		}

		memset(diss, -1, sizeof diss);
		memset(pre, 0, sizeof pre);
		diss[startx][starty] = 1, pre[startx][starty] = -1;
		q.push(pa(startx, starty));

		while (!q.empty()) {
			int num = q.size();
			while (num--) {
				pair<int, int>x = q.front();
				q.pop();
				for (int i = 0; i < 4; i++) {
					int xx = x.ff * 2 - 1 + dd[i][0], yy = x.ss + dd[i][1];
					int xxx = x.ff + dd[i][0], yyy = x.ss + dd[i][1];
					if (i == 2)yyy++;
					if (check(xx, yy) && b[xx][yy] && diss[xxx][yyy] == -1) {
						diss[xxx][yyy] = root;
						pre[xxx][yyy] = (x.ff - 1)*width + x.ss;

						if (xxx == endx&&yyy == endy)
							find = 1;

						q.push(pa(xxx, yyy));
					}
				}
				if (find)break;
			}
			if (find)break;

			root++;
		}

		if (find) {
			int xx = endx, yy = endy;
			while (pre[xx][yy] != -1) {
				ans[xx][yy]=diss[xx][yy];
				v.push_back(pa(xx, yy));

				int t = pre[xx][yy];
				xx = t / width + 1, yy = t % width;
			}

			ans[startx][starty]=1;
			v.push_back(pa(startx, starty));

			min_step = root;

			break;
		}
	}
}

void Map::print(int x, int y, int type) {
	switch (type) {
	case 0:
		putimage(x, y, &p0000);
		break;
	case 1:
		putimage(x, y, &p0001);
		break;
	case 2:
		putimage(x, y, &p0010);
		break;
	case 3:
		putimage(x, y, &p0011);
		break;
	case 4:
		putimage(x, y, &p0100);
		break;
	case 5:
		putimage(x, y, &p0101);
		break;
	case 6:
		putimage(x, y, &p0110);
		break;
	case 7:
		putimage(x, y, &p0111);
		break;
	case 8:
		putimage(x, y, &p1000);
		break;
	case 9:
		putimage(x, y, &p1001);
		break;
	case 10:
		putimage(x, y, &p1010);
		break;
	case 11:
		putimage(x, y, &p1011);
		break;
	case 12:
		putimage(x, y, &p1100);
		break;
	case 13:
		putimage(x, y, &p1101);
		break;
	case 14:
		putimage(x, y, &p1110);
		break;
	case 15:
		putimage(x, y, &p1111);
	}
}

// Menu

Menu::Menu() {
	loadimage(&start, L"image\\start.gif");
	loadimage(&start_, L"image\\start_.gif");
	loadimage(&setting, L"image\\setting.gif");
	loadimage(&setting_, L"image\\setting_.gif");
	loadimage(&exit, L"image\\exit.gif");
	loadimage(&exit_, L"image\\exit_.gif");

	loadimage(&setting_background, L"image\\setting_background.png");
	loadimage(&wasd, L"image\\wasd.png");
	loadimage(&pesc, L"image\\pesc.png");
	loadimage(&setting_background_ingame, L"image\\setting_background_ingame.png");

	loadimage(&game_background_1, L"image\\game_background_1.png");
	loadimage(&game_background_2, L"image\\game_background_2.png");
	loadimage(&circle, L"image\\circle.png");
	loadimage(&heart, L"image\\heart.png");
	loadimage(&__2, L"image\\2.png");
	loadimage(&__3, L"image\\3.png");

	loadimage(&back, L"image\\back.png");
	loadimage(&game_start, L"image\\game_start.png");
	loadimage(&rand_, L"image\\rand.png");

	loadimage(&_0, L"image\\_0.png");
	loadimage(&_1, L"image\\_1.png");
	loadimage(&_2, L"image\\_2.png");
	loadimage(&_3, L"image\\_3.png");
	loadimage(&_4, L"image\\_4.png");
	loadimage(&_5, L"image\\_5.png");
	loadimage(&_6, L"image\\_6.png");
	loadimage(&_7, L"image\\_7.png");
	loadimage(&_8, L"image\\_8.png");
	loadimage(&_9, L"image\\_9.png");

	loadimage(&gameover, L"image\\gameover.png");
	loadimage(&return_, L"image\\return.png");
	loadimage(&victory_, L"image\\victory.png");
}

void Menu::get_kb(const int do_type, const int pause_type) {
	command.get_kb(do_type, pause_type);
}

int Menu::victory(const int times, const int step, const int min_step) {
	while (1) {
		command.get_kb(1, 1);

		pair<int, int>pos = command.get_pos();
		bool hit = command.get_hit();

		setfillcolor(BLACK);
		solidrectangle(0, 0, WIDTH, HEIGHT);

		putimage(WIDTH / 4, HEIGHT / 4, &victory_);

		if (WIDTH / 4 + 56 <= pos.ff&&pos.ff <= 164 + WIDTH / 4 && 294 + HEIGHT / 4 <= pos.ss&&pos.ss <= 320 + HEIGHT / 4) {
			putimage(WIDTH / 4 + 49, HEIGHT / 4 + 289, &return_);
			if (hit)
				return 1;// return
		}

		if (WIDTH / 4 + 492 <= pos.ff&&pos.ff <= 600 + WIDTH / 4 && 295 + HEIGHT / 4 <= pos.ss&&pos.ss <= 321 + HEIGHT / 4) {
			putimage(WIDTH / 4 + 485, HEIGHT / 4 + 290, &gameover);
			if (hit)
				return 2;// gamemover
		}

		int l = times / 20, r = times % 20 * 50;

		// 打印毫秒
		put_number(WIDTH / 4 + 481, HEIGHT / 4 + 82, r % 10);
		if (r / 10)
			put_number(WIDTH / 4 + 469, HEIGHT / 4 + 82, r / 10 % 10);
		if (r / 100)
			put_number(WIDTH / 4 + 457, HEIGHT / 4 + 82, r / 100);

		// 打印秒
		put_number(WIDTH / 4 + 387, HEIGHT / 4 + 82, l % 10);
		if (l / 10)
			put_number(WIDTH / 4 + 375, HEIGHT / 4 + 82, l / 10 % 10);
		if (l / 100)
			put_number(WIDTH / 4 + 363, HEIGHT / 4 + 82, l / 100);

		// 打印步数
		put_number(WIDTH / 4 + 451, HEIGHT / 4 + 144, step % 10);
		if (step / 10)
			put_number(WIDTH / 4 + 439, HEIGHT / 4 + 144, step / 10 % 10);
		if (step / 100)
			put_number(WIDTH / 4 + 427, HEIGHT / 4 + 144, step / 100);

		// 打印最小步数
		put_number(WIDTH / 4 + 451, HEIGHT / 4 + 206, min_step % 10);
		if (min_step / 10)
			put_number(WIDTH / 4 + 439, HEIGHT / 4 + 206, min_step / 10 % 10);
		if (min_step / 100)
			put_number(WIDTH / 4 + 427, HEIGHT / 4 + 206, min_step / 100);

		FlushBatchDraw();
	}
	return 0;
}

void Menu::put_number(int x, int y, int num) {
	switch (num) {
	case 0:
		putimage(x, y, &_0);
		break;
	case 1:
		putimage(x, y, &_1);
		break;
	case 2:
		putimage(x, y, &_2);
		break;
	case 3:
		putimage(x, y, &_3);
		break;
	case 4:
		putimage(x, y, &_4);
		break;
	case 5:
		putimage(x, y, &_5);
		break;
	case 6:
		putimage(x, y, &_6);
		break;
	case 7:
		putimage(x, y, &_7);
		break;
	case 8:
		putimage(x, y, &_8);
		break;
	case 9:
		putimage(x, y, &_9);
	}
}

int Menu::print_menu0() {
	setfillcolor(BLACK);
	solidrectangle(0, 0, WIDTH, HEIGHT);

	setbkcolor(BLACK);
	settextcolor(WHITE);

	settextstyle(80, 0, _T("华文行楷"));
	outtextxy(300, 100, _T("随机迷宫小游戏"));

	pair<int, int>pos = command.get_pos();



	const int left = 460;
	const int up = 300;
	const int high = 50;// 161*37

	if (left <= pos.ff&&pos.ff <= 161 + left&&up <= pos.ss&&pos.ss <= 37 + up) {
		putimage(left, up, &start_);
		if (command.get_hit())
			return 1;
	}
	else
		putimage(left, up, &start);

	if (left <= pos.ff&&pos.ff <= 161 + left&&up + high + 37 <= pos.ss&&pos.ss <= 2 * 37 + high + up) {
		putimage(left, up + high + 37, &setting_);
		if (command.get_hit())
			return 2;
	}
	else
		putimage(left, up + high + 37, &setting);

	if (left <= pos.ff&&pos.ff <= 161 + left&&up + 2 * high + 2 * 37 <= pos.ss&&pos.ss <= 3 * 37 + 2 * high + up) {
		putimage(left, up + 2 * high + 2 * 37, &exit_);
		if (command.get_hit())
			return 3;
	}
	else
		putimage(left, up + 2 * high + 2 * 37, &exit);

	return 0;
}

int Menu::print_menu1(int&maze_type, int&gamemode, int&hard, int&height, int&width) {
	pair<int, int>pos = command.get_pos();
	bool hit = command.get_hit();

	const int aw[4][2] = { 0,0,20,50,20,50,21,49 };//////
	const int ah[4][2] = { 0,0,20,50,20,50,22,50 };//////

	if (gamemode == 1) {
		putimage(0, 0, &game_background_1);

		if (hard == 2)
			putimage(45, 445, &__2);
		else if (hard == 3)
			putimage(45, 445, &__3);
	}
	else
		putimage(0, 0, &game_background_2);

	if (maze_type == 2)
		putimage(43, 168, &circle);
	else if (maze_type == 3)
		putimage(43, 168, &heart);

	if (715 <= pos.ff&&pos.ff <= 904 && 283 <= pos.ss&&pos.ss <= 351) {
		putimage(715, 283, &rand_);
		if (hit) {
			width = rand() % (aw[maze_type][1] - aw[maze_type][0]) + aw[maze_type][0];
			height = rand() % (ah[maze_type][1] - ah[maze_type][0]) + ah[maze_type][0];
			if (maze_type == 2) {
				if (height % 2)
					height--;
				width = height;
			}
			else if (maze_type == 3) {
				if (height % 2)
					height--;
				width = height - 1;
			}
		}
	}// rand

	if (aw[maze_type][0] > width || width > aw[maze_type][1])
		width = aw[maze_type][0];
	if (ah[maze_type][0] > height || height > ah[maze_type][1])
		height = ah[maze_type][0];

	if (hit && 133 <= pos.ff&&pos.ff <= 302 && 317 <= pos.ss&&pos.ss <= 327) {
		width = int(((pos.ff - 133) / 169.0)*(aw[maze_type][1] - aw[maze_type][0]) + 0.5) + aw[maze_type][0];
		if (maze_type == 2) {
			if (width % 2)
				width--;
			height = width;
		}
		else if (maze_type == 3) {
			if (width % 2 == 0)
				width--;
			height = width + 2;
		}
			
	}

	if (hit && 444 <= pos.ff&&pos.ff <= 613 && 317 <= pos.ss&&pos.ss <= 327) {
		height = int(((pos.ff - 444) / 169.0)*(ah[maze_type][1] - ah[maze_type][0]) + 0.5) + ah[maze_type][0];
		if (maze_type == 2) {
			if (height % 2)
				height--;
			width = height;
		}
		else if (maze_type == 3) {
			if (height % 2 == 0)
				height--;
			width = height - 2;
		}
	}

	solidcircle(133 + (width - aw[maze_type][0]) * 169 / (aw[maze_type][1] - aw[maze_type][0]), 322, 6);
	solidcircle(444 + (height - ah[maze_type][0]) * 169 / (ah[maze_type][1] - ah[maze_type][0]), 322, 6);

	put_number(321, 315, width / 10);
	put_number(334, 315, width % 10);

	put_number(632, 315, height / 10);
	put_number(645, 315, height % 10);


	if (hit && 127 <= pos.ff&&pos.ff <= 305 && 220 <= pos.ss&&pos.ss <= 248) {
		maze_type = 1;
		width = aw[maze_type][0], height = ah[maze_type][0];
	}

	if (hit && 325 <= pos.ff&&pos.ff <= 503 && 220 <= pos.ss&&pos.ss <= 248){
		maze_type = 2;
		width = aw[maze_type][0], height = ah[maze_type][0];
	}

	if (hit && 523 <= pos.ff&&pos.ff <= 701 && 219 <= pos.ss&&pos.ss <= 247) {
		maze_type = 3;
		width = aw[maze_type][0], height = ah[maze_type][0];
	}


	if (hit && 158 <= pos.ff&&pos.ff <= 336 && 397 <= pos.ss&&pos.ss <= 425)
		gamemode = 1;
	if (hit && 326 <= pos.ff&&pos.ff <= 540 && 397 <= pos.ss&&pos.ss <= 425)
		gamemode = 2;

	if (gamemode == 1) {
		if (hit && 127 <= pos.ff&&pos.ff <= 305 && 494 <= pos.ss&&pos.ss <= 522)
			hard = 1;
		if (hit && 325 <= pos.ff&&pos.ff <= 503 && 494 <= pos.ss&&pos.ss <= 522)
			hard = 2;
		if (hit && 523 <= pos.ff&&pos.ff <= 701 && 493 <= pos.ss&&pos.ss <= 521)
			hard = 3;
	}

	if (hit && 35 <= pos.ff&&pos.ff <= 176 && 87 <= pos.ss&&pos.ss <= 126)
		return 1;// choose->setting

	if (635 <= pos.ff&&pos.ff <= 819 && 559 <= pos.ss&&pos.ss <= 612) {
		putimage(635, 559, &back);
		if (hit)
			return 2;// back to menu0
	}

	if (837 <= pos.ff&&pos.ff <= 1021 && 558 <= pos.ss&&pos.ss <= 611) {
		putimage(837, 558, &game_start);
		if (hit)
			return 3;// game start!!
	}

	return 0;
}

int Menu::print_menu2(int &do_type, int &pause_type, const int type) {
	// type指在那里打开的 1为设置中 2为游戏中

	pair<int, int>pos = command.get_pos();
	bool hit = command.get_hit();

	if (type == 1)
		putimage(0, 0, &setting_background);
	else
		putimage(0, 0, &setting_background_ingame);

	if (do_type == 2)
		putimage(47, 165, &wasd);
	if (pause_type == 2)
		putimage(53, 307, &pesc);

	if (hit && 158 <= pos.ff&&pos.ff <= 336 && 220 <= pos.ss&&pos.ss <= 248)
		do_type = 1;
	if (hit && 362 <= pos.ff&&pos.ff <= 540 && 220 <= pos.ss&&pos.ss <= 248)
		do_type = 2;

	if (hit && 164 <= pos.ff&&pos.ff <= 342 && 362 <= pos.ss&&pos.ss <= 390)
		pause_type = 1;
	if (hit && 368 <= pos.ff&&pos.ff <= 546 && 362 <= pos.ss&&pos.ss <= 390)
		pause_type = 2;

	if (hit && 905 <= pos.ff&&pos.ff <= 1046 && 90 <= pos.ss&&pos.ss <= 129 && type == 1)
		return 1;// choose->maze
	if (847 <= pos.ff&&pos.ff <= 1008 && 566 <= pos.ss&&pos.ss <= 603) {
		putimage(837, 558, &back);
		if (hit)
			return 2;// back to menu0
	}

	return 0;
}

// Game

Game::Game() {
	gamemode = 1;
	hard = 1;
	do_type = 1;
	pause_type = 1;
	height = width = 0;
	maze_type = 1;
	tt = 0;

	srand((unsigned)time(NULL));
}

Game::~Game() {

}

void Game::run_main() {
	int imano = 0;
	pair<int, int>pos;// 像素坐标
	pair<int, int>maze_pos;// 迷宫坐标
	pair<int, int>pp;// 迷宫左上点坐标
	pair<int, int>end;// 终点
	int step = 0, range;

	BeginBatchDraw();

	while (1) {
		switch (imano) {
		case 0:
			while (1) {
				menu.get_kb(do_type, pause_type);

				imano = menu.print_menu0();

				FlushBatchDraw();

				if (imano == 3)
					return;
				if (imano)
					break;
			}
			break;
		case 1:
			while (1) {
				menu.get_kb(do_type, pause_type);

				int num = menu.print_menu1(maze_type, gamemode, hard, height, width);

				if (num == 1)
					imano = 2;
				else if (num == 2)
					imano = 0;
				else if (num == 3) {
					if (gamemode == 1)
						imano = 3;
					else
						imano = 4;
				}

				FlushBatchDraw();

				if (imano != 1)
					break;
			}
			break;
		case 2:
			while (1) {
				menu.get_kb(do_type, pause_type);

				int num = menu.print_menu2(do_type, pause_type, 1);

				if (num == 1)
					imano = 1;
				else if (num == 2)
					imano = 0;

				FlushBatchDraw();

				if (imano != 2)
					break;
			}
			break;
		case 3:
			step = 0, tt = 0;
			while (1) {
				long t1 = GetTickCount();

				if (!map.get_ok()) {
					map.set_height(height);
					map.set_width(width);
					map.set_type(maze_type);

					map.make_new_map();
					map.set_ok(true);

					pos.ff = map.get_starty();
					maze_pos.ff = pos.ff;
					pos.ff = (WIDTH - width * 10) / 2 + (pos.ff - 1) * 10 + 5;

					pos.ss = map.get_startx();
					maze_pos.ss = pos.ss;
					pos.ss = (HEIGHT - height * 10) / 2 + (pos.ss - 1) * 10 + 5;

					pp.ff = (WIDTH - width * 10) / 2;
					pp.ss = (HEIGHT - height * 10) / 2;

					end.ff = map.get_endy(), end.ss = map.get_endx();

					if (hard == 1)
						range = 5 + 1;
					else if (hard == 2)
						range = 3 + 1;
					else
						range = 2 + 1;
				}

				menu.get_kb(do_type, pause_type);

				pair<int, int>mouse_pos = menu.get_pos();
				bool hit = menu.get_hit();
				int doo = menu.get_doo();

				int dx = pos.ff - (maze_pos.ff - 1) * 10 - pp.ff;
				int dy = pos.ss - (maze_pos.ss - 1) * 10 - pp.ss;

				int cango = 0;
				for (int i = 0; i < 4; i++) {
					int xx = 2 * maze_pos.ss - 1 + dd[i][0], yy = maze_pos.ff + dd[i][1];
					if (map.get_b(xx, yy))
						cango += (1 << i);// left down right up
				}

				if (doo & 1) {// left
					dx -= speed;
					if (dx < person_r + wall_r) {
						if (cango & 1) {
							pos.ff -= speed;
							if (dx < 0) {
								maze_pos.ff--;
								step++;
							}
						}
						else
							pos.ff = pp.ff + (maze_pos.ff - 1) * 10 + person_r + wall_r;
					}
					else
						pos.ff -= speed;
				}
				if (doo & 2) {// right
					dx += speed;
					if (dx > 10 - person_r - wall_r) {
						if (cango & 4) {
							pos.ff += speed;
							if (dx > 10) {
								maze_pos.ff++;
								step++;
							}
						}
						else
							pos.ff = pp.ff + maze_pos.ff * 10 - person_r - wall_r;
					}
					else
						pos.ff += speed;
				}
				if (doo & 4) {// up
					dy -= speed;
					if (dy < person_r + wall_r) {
						if (cango & 8) {
							pos.ss -= speed;
							if (dy < 0) {
								maze_pos.ss--;
								step++;
							}
						}
						else
							pos.ss = pp.ss + (maze_pos.ss - 1) * 10 + person_r + wall_r;
					}
					else
						pos.ss -= speed;
				}
				if (doo & 8) {// down
					dy += speed;
					if (dy > 10 - person_r - wall_r) {
						if (cango & 2) {
							pos.ss += speed;
							if (dy > 10) {
								maze_pos.ss++;
								step++;
							}
						}
						else
							pos.ss = pp.ss + maze_pos.ss * 10 - person_r - wall_r;
					}
					else
						pos.ss += speed;
				}

				if (doo & 16) {
					while (1) {
						menu.get_kb(do_type, pause_type);
						int num = menu.print_menu2(do_type, pause_type, 2);

						FlushBatchDraw();

						if (num == 2)
							break;
					}
					continue;
				}

				setfillcolor(GRAY);
				solidrectangle(0, 0, WIDTH, HEIGHT);

				if (maze_pos.ff == end.ff&&maze_pos.ss == end.ss) {
					int num = menu.victory(tt, step, map.get_minstep());
					if (num == 1) {
						imano = 0;
						break;
					}
					else
						return;
				}

				pair<int, int> p1, p2;
				p1 = pa(maze_pos.ff - range, maze_pos.ss - range);
				p2 = pa(maze_pos.ff + range, maze_pos.ss + range);

				for (int i = p1.ff; i <= p2.ff; i++)
					if (i >= 0 && i <= width + 1)
						for (int o = p1.ss; o <= p2.ss; o++)
							if (o >= 0 && o <= height + 1)
								if (map.get_a(o, i)) {
									int type = 0;

									for (int p = 0; p < 4; p++) {
										int xx = 2 * o - 1 + dd[p][0], yy = i + dd[p][1];
										if (map.get_b(xx, yy))
											type += (1 << p);
									}

									map.print(pp.ff + (i - 1) * 10, pp.ss + (o - 1) * 10, type);
								}
								else if (map.get_edge(o, i))
									map.print(pp.ff + (i - 1) * 10, pp.ss + (o - 1) * 10, 0);

				setfillcolor(RED);
				solidcircle(pos.ff, pos.ss, person_r);

				setfillcolor(BLACK);
				solidrectangle(0, 0, WIDTH, pos.ss - range * 10 + 1);
				solidrectangle(0, pos.ss + range * 10 - 1, WIDTH, HEIGHT);
				solidrectangle(0, pos.ss - range * 10, pos.ff - range * 10 + 1, pos.ss + range * 10);
				solidrectangle(pos.ff + range * 10 - 1, pos.ss - range * 10, WIDTH, pos.ss + range * 10);
				
				draw_circle(pos, range * 10);

				putimage(pp.ff + (end.ff - 2) * 10, pp.ss + (end.ss - 2) * 10, &map.get_qizi(tt / 4 % 4, 1), SRCAND);
				putimage(pp.ff + (end.ff - 2) * 10, pp.ss + (end.ss - 2) * 10, &map.get_qizi(tt / 4 % 4, 0), SRCINVERT);


				// 秒表
				putimage(WIDTH - 250, 0, &map.get_time_());

				char sss[10];
				wchar_t s[10];
				sprintf_s(sss, sizeof sss, "%3d", tt / 20);
				change(sss, s);

				settextstyle(30, 0, _T("黑体"));
				setbkcolor(WHITE);
				settextcolor(BLACK);
				outtextxy(WIDTH - 140, 45, s);

				setfillcolor(WHITE);
				solidrectangle(850, 160, 1050, 162);


				// step
				draw_girl(doo);
				sprintf_s(sss, sizeof sss, "%3d", step);
				change(sss, s);

				setbkcolor(BLACK);
				settextcolor(WHITE);
				outtextxy(950, 130, s);

				FlushBatchDraw();

				long t2 = GetTickCount();
				if (t2 - t1 < 50)
					Sleep(50 - t2 + t1);

				tt++;
			}
			draw_circle(pos, -1);// clear
			map.set_ok(false);

			break;
		case 4:
			print_watching();

			imano = 0;
		}
	}
	EndBatchDraw();
}

void Game::draw_circle(pair<int, int>pos, int r) {
	static vector<int>vv;

	if (r == -1) {
		vv.clear();
		return;
	}

	if (vv.size() == 0)
		for (int i = 0; i <= r; i++)
			for (int o = r; o >= 0; o--) {
				if (dis((r - i) * 10 + 5, (r - o) * 10 + 5, 0, 0) - r*r * 10 * 10 >= 0) {
					vv.push_back(o);
					break;
				}
			}

	setfillcolor(BLACK);
	for (int i = 0; i < vv.size(); i++) {
		solidrectangle(pos.ff - r, pos.ss - r + i, pos.ff - r + vv[i], pos.ss - r + i + 1);
		solidrectangle(pos.ff + r - vv[i], pos.ss - r + i, pos.ff + r, pos.ss - r + i + 1);
		solidrectangle(pos.ff - r, pos.ss + r - i - 1, pos.ff - r + vv[i], pos.ss + r - i);
		solidrectangle(pos.ff + r - vv[i], pos.ss + r - i - 1, pos.ff + r, pos.ss + r - i);
	}
}

void Game::draw_girl(const int x) {
	pair<int, int>pos = pa(900, 120);
	static int times = 0;

	setfillcolor(SHALLOW_YELLOW);
	solidcircle(pos.ff, pos.ss - 14, 10);// head

	setlinecolor(SHALLOW_YELLOW);
	for (int i = 0; i < 3; i++)
		line(pos.ff - 1 + i, pos.ss, pos.ff - 1 + i, pos.ss - 6);// neck

	setfillcolor(PINK);
	solidrectangle(pos.ff - 8, pos.ss, pos.ff + 8, pos.ss + 18);

	const int dd_foot[6] = { 1,3,5,7,5,3 };
	const int dd_leg1[6] = { -5,-3,1,3,1,-3 };
	const int dd_leg2[6] = { 24,24,20,20,18,22 };
	int ans = times % 18 / 3;

	for (int i = 0; i < 3; i++) {
		line(pos.ff + i, pos.ss + 20, pos.ff + i - dd_foot[ans], pos.ss + 40);
		line(pos.ff + i, pos.ss + 20, pos.ff + i + dd_foot[ans], pos.ss + 40);
		line(pos.ff + i, pos.ss + 5, pos.ff + i + dd_leg1[ans], pos.ss + dd_leg2[ans]);
	}

	setfillcolor(PINK);
	solidellipse(pos.ff - 13, pos.ss + 18, pos.ff + 13, pos.ss + 28);// shirt

	if (0 < x&&x < 16)
		times++;
}

void Game::print_watching() {
	long t1 = GetTickCount();

	pair<int, int>pp;
	pp.ff = (WIDTH - width * 10) / 2;
	pp.ss = (HEIGHT - height * 10) / 2;

	setfillcolor(BLACK);
	solidrectangle(0, 0, WIDTH, HEIGHT);

	if (!map.get_ok()) {
		map.set_height(height);
		map.set_width(width);
		map.set_type(maze_type);

		map.make_new_map();
		map.set_ok(true);
	}

	bool ok = false;
	int a = 1, b = 1,tt=map.get_sizev()-1;

	while (1) {

		if(!ok)
			for (int i = a; i <= height ; i++) {
				for (int o = b; o <= width ; o++)
					if (map.get_a(i, o)) {
						int type = 0;
						for (int p = 0; p < 4; p++) {
							int xx = 2 * i - 1 + dd[p][0], yy = o + dd[p][1];
							if (map.get_b(xx, yy))
								type += (1 << p);
						}
						map.print(pp.ff + (o - 1) * 10, pp.ss + (i - 1) * 10, type);
						
						b = o + 1;
						ok = true;

						break;
					}

				
				if (ok) {
					a = i;
					ok = false;

					break;
				}

				b = 0;

				if (ok == false && i == height) {
					a = height;
					b = width;
				}
			}

		if (a == height&&b == width&&tt >= 0) {
			setfillcolor(PURPLE);

			int l = pp.ff + (map.get_v(tt).ss - 1) * 10;
			int t = pp.ss + (map.get_v(tt).ff - 1) * 10;
			int r = pp.ff + map.get_v(tt).ss * 10;
			int d = pp.ss + map.get_v(tt).ff * 10;

			solidrectangle(l, t, r, d);

			tt--;
		}

		menu.get_kb(do_type, pause_type);
		pair<int, int>pos = menu.get_pos();
		bool hit = menu.get_hit();


		if (800 <= pos.ff&&pos.ff <= 961 && 660 <= pos.ss&&pos.ss <= 697) {
			putimage(800, 660, &menu.get_exit_());
			if (hit)
				break;
		}
		else
			putimage(800, 660, &menu.get_exit());


		FlushBatchDraw();

		long t2 = GetTickCount();

		//if (t2 - t1 < 100)
			//Sleep(100 - t2 + t1);
		Sleep(40);
	}

	map.set_ok(false);

}