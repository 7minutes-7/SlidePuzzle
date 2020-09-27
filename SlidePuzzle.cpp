#include <bangtal.h>
#include <iostream>
#include <ctime>

using namespace bangtal;
using namespace std;

void swap(ObjectPtr game_board[], ScenePtr scene, int startx, int starty, int diff, int row, int* swap1, int* blank) {
	game_board[*swap1]->locate(scene, startx + (*blank % row) * diff, starty - (*blank / row) * diff);
	game_board[*blank]->locate(scene, startx + (*swap1 % row) * diff, starty - (*swap1 / row) * diff);

	auto object = game_board[*swap1];
	game_board[*swap1] = game_board[*blank];
	game_board[*blank] = object;
	*blank = *swap1;
}

void mix(ObjectPtr game_board[], ScenePtr scene, int startx, int starty, int diff, int row, int* blank) {
	int j = 0;
	do {
		switch (rand() % 4) {
		case 0: j = *blank - 1; break;
		case 1: j = *blank + 1; break;
		case 2: j = *blank - row; break;
		case 3: j = *blank + row; break;
		}
	} while (j < 0 || j > row * row - 1 || !((j % row > 0 && j - 1 == *blank) ||
		(j % row < row - 1 && j + 1 == *blank) ||
		(j > row - 1 && j - row == *blank) ||
		(j < row * row - row && j + row == *blank)));
	swap(game_board, scene, startx, starty, diff, row, &j, blank);
}

void play(string food, ObjectPtr game_board[], ScenePtr scene, ObjectPtr init_board[], int startx, int starty, int diff, int row, int* blank) {
	time_t start_time = time(NULL);
	*blank = row * row - 1;

	for (int i = 0; i < row * row; i++) { //타일 위치
		string filename = "Images/" + food + to_string(i + 1) + ".png";
		game_board[i] = Object::create(filename, scene, startx + (i % row) * diff, starty - (i / row) * diff);
		init_board[i] = game_board[i];

		game_board[i]->setOnMouseCallback([=](ObjectPtr object, int, int, MouseAction)->bool { //타일 맞추기
			int j = 0;
			for (j = 0; j < row * row; j++) {
				if (game_board[j] == object)
					break;
			}
			cout << j << endl;

			if ((j % row > 0 && j - 1 == *blank) ||
				(j % row < row - 1 && j + 1 == *blank) ||
				(j > row - 1 && j - row == *blank) ||
				(j < row * row - row && j + row == *blank))
			{
				swap(game_board, scene, startx, starty, diff, row, &j, blank);

				int k;
				for (k = 0; k < row * row; k++) {
					if (game_board[k] != init_board[k]) break;
				}
				if (k == row * row) {
					auto end_time = time(NULL);
					cout << difftime(end_time, start_time) << endl;
					string buf = food + "를 주문하는데 " + to_string(int(difftime(end_time, start_time))) + "초 걸렸습니다!";
					showMessage(buf.c_str());
				}
			}
			return true;
			});
	}
	game_board[*blank]->hide();
}

int main()
{
	srand((unsigned int)time(NULL));
	time_t start_time;
	auto timer = Timer::create(0.1f);

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	auto title = Scene::create("퍼즐 게임", "Images/title.png");
	auto b_title = Scene::create("버거를 완성하라", "Images/burger.png");
	auto n_title = Scene::create("너겟을 완성하라", "Images/nuggets.png");
	auto f_title = Scene::create("감튀를 완성하라", "Images/fries.png");

	auto burger_icon = Object::create("Images/burger_icon.png", title, 135, 400);
	auto nuggets_icon = Object::create("Images/nuggets_icon.png", title, 133, 250);
	auto fries_icon = Object::create("Images/fries_icon.png", title, 150, 50);

	auto b_background = Object::create("Images/배경_burger.jpg", b_title, 0, 0, false);
	auto n_background = Object::create("Images/배경_nuggets.jpg", n_title, 0, 0, false);
	auto f_background = Object::create("Images/배경_fries.jpg", f_title, 0, 0, false);

	auto b_start = Object::create("Images/start.png", b_title, 500, 320);  b_start->setScale(2.0f);
	auto n_start = Object::create("Images/start.png", n_title, 500, 320);  n_start->setScale(2.0f);
	auto f_start = Object::create("Images/start.png", f_title, 500, 320);  f_start->setScale(2.0f);

	burger_icon->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		b_title->enter();
		return true;
		});
	nuggets_icon->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		n_title->enter();
		return true;
		});
	fries_icon->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		f_title->enter();
		return true;
		});

	ObjectPtr b_game_board[25];
	ObjectPtr b_init_board[25];
	ObjectPtr n_game_board[16];
	ObjectPtr n_init_board[16];
	ObjectPtr f_game_board[9];
	ObjectPtr f_init_board[9];

	int count = 0;
	int row;
	int blank;
	int startx; int starty; int diff;

	b_start->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		string food = "burger";
		row = 5;
		startx = 291; starty = 524; diff = 128;
		b_start->hide();
		b_background->show();

		play(food, b_game_board, b_title, b_init_board, startx, starty, diff, row, &blank);
		timer->setOnTimerCallback([&](TimerPtr t)->bool {
			mix(b_game_board, b_title, startx, starty, diff, row, &blank);
			cout << "timeout!" << count + 1 << endl;
			count++;
			if (count < 300) {
				t->set(0.05f);
				t->start();
			}
			return true;
			});
		timer->start();
		start_time = time(NULL);
		return true;
		});

	n_start->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		string food = "nuggets";
		row = 4;
		startx = 296; starty = 524; diff = 160;
		n_start->hide();
		n_background->show();

		play(food, n_game_board, n_title, n_init_board, startx, starty, diff, row, &blank);

		timer->setOnTimerCallback([&](TimerPtr t)->bool {
			mix(n_game_board, n_title, startx, starty, diff, row, &blank);
			cout << "timeout!" << count + 1 << endl;
			count++;
			if (count < 200) {
				t->set(0.05f);
				t->start();
			}
			return true;
			});
		timer->start();
		start_time = time(NULL);

		return true;
		});

	f_start->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		string food = "fries";
		row = 3;
		startx = 324; starty = 460; diff = 210;
		f_start->hide();
		f_background->show();

		play(food, f_game_board, f_title, f_init_board, startx, starty, diff, row, &blank);

		timer->setOnTimerCallback([&](TimerPtr t)->bool {
			mix(f_game_board, f_title, startx, starty, diff, row, &blank);
			cout << "timeout!" << count + 1 << endl;
			count++;
			if (count < 100) {
				t->set(0.05f);
				t->start();
			}
			return true;
			});
		timer->start();
		start_time = time(NULL);
		return true;
		});

	startGame(title);
	return 0;
}