# include <Siv3D.hpp> // OpenSiv3D v0.6.3

struct GameData
{
	int32 score = 0, highscore = 0, prescore = 0;
	int32 num = 0, ballnum = 10;
	int32 stageNum = 1, allStage = 1;
	SecondsF timeSet = 20.0s;
};
using App = SceneManager<String, GameData>;
class Title :public App::Scene {
private:
	Rect Start{ 325, 460, 150, 80 };
public:
	Title(const InitData& init) :IScene(init) {
	}
	void update() FMT_OVERRIDE {

		if (MouseL.down() && Start.mouseOver())
		{
			getData().allStage = getData().num * 2 + 1;
			getData().ballnum = getData().stageNum * getData().allStage + 1;
			getData().score = 0;
			changeScene(U"Game");
		}

	}
	void draw() const override
	{
		const Polygon polygon1
		{
			Vec2{ 550, 225 }, Vec2{ 600, 300 }, Vec2{ 550, 375 }, Vec2{ 575, 300 }
		};
		const Polygon polygon2
		{
			//中心(400,300)
			Vec2{ 250, 225 }, Vec2{ 225, 300 }, Vec2{ 250, 375 }, Vec2{ 200, 300 }
		};
		polygon1.draw(Palette::Blue);
		polygon2.draw(Palette::Blue);
		if (polygon1.mouseOver() && getData().num <= 2)//Right
		{
			polygon1.draw(Palette::Skyblue);
			if (MouseL.down())
			{
				getData().num++;

			}
		}
		else
		{
			polygon1.draw(Palette::Blue);
		}
		if (polygon2.mouseOver() && getData().num >= 1)//Left
		{
			polygon2.draw(Palette::Skyblue);
			if (MouseL.down())
			{
				getData().num--;

			}
		}
		else
		{
			polygon2.draw(Palette::Blue);
		}
		Scene::SetBackground(Palette::Violet);
		FontAsset(U"TitleFont")(U"Color Game").drawAt(400, 100);
		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().prescore)).draw(590, 490);
		FontAsset(U"ScoreFont")(U"HighScore: {}"_fmt(getData().highscore)).draw(520, 540);
		if (getData().num == 0) {
			FontAsset(U"ScoreFont")(U"EASY").drawAt(400, 300);
		}
		if (getData().num == 1) {
			FontAsset(U"ScoreFont")(U"NORMAL").drawAt(400, 300);
		}
		if (getData().num == 2) {
			FontAsset(U"ScoreFont")(U"HALD").drawAt(400, 300);
		}
		if (getData().num == 3) {
			FontAsset(U"ScoreFont")(U"CHALLENGE").drawAt(400, 300);
		}
		if (Start.mouseOver()) {
			Start.draw(Palette::Skyblue);
		}
		else {
			Start.draw(Palette::Blue);
		}
		FontAsset(U"ScoreFont")(U"START").drawAt(400, 500);
	}
};
class Game : public App::Scene {
private:
	Timer timer{ getData().timeSet, StartImmediately::Yes };
	ColorF backcolor = Palette::Green;
	bool gamefinish = false;
	bool clear = false;
	Circle player1{ 300, 500, 10 };
	Array<Rect>goals;
	Array<ColorF>goalcolor;
	int32 goalColorNum = 0;
	Array<int32>goalX, goalY;
	bool allowance = false;
	int32 col1 = 0, col2 = 0, col3 = 0;
	Vec2 RandomGoal()
	{
		return RandomVec2({ 1, 579 }, { 1, 579 });
	}
	ColorF setColor(int x) {
		ColorF color;
		if (x == 1)color = Palette::Darkcyan;
		if (x == 2)color = Palette::Darkgreen;
		if (x == 3)color = Palette::Darkslateblue;
		return color;
	}
	static constexpr double playerSpeed = 300.0;
public:
	Game(const InitData& init) : IScene(init) {
		for (auto i : step(getData().ballnum)) {
			goalX << RandomGoal().x;
			goalY << RandomGoal().y;
			while (allowance == false) {
				allowance = true;
				for (int j = 0; j < i; j++) {
					if (abs(goalX[i] - goalX[j]) < 21 && abs(goalY[i] - goalY[j]) < 21 || abs(goalX[i] - 290) < 21 && abs(goalY[i] - 490) < 21) {
						allowance = false;
						goalX[i] = RandomGoal().x;
						goalY[i] = RandomGoal().y;
						break;
					}
				}
			}
			allowance = false;
			goals << Rect(goalX[i], goalY[i], 20, 20);
		}
		for (const auto& goal : goals)
		{
			if (Max({ col1,col2,col3 })>col1+col2+col3- Max({ col1,col2,col3 })) {
				if (Max({ col1,col2,col3 }) == col1) {
					goalcolor << setColor(2);
					col2++;
				}
				else if (Max({ col1,col2,col3 }) == col2) {
					goalcolor << setColor(3);
					col3++;
				}
				else{
					goalcolor << setColor(1);
					col1++;
				}
			}
			else {
				if ((goal.x + goal.y) % 3 == 0) {
					goalcolor << setColor(1);
					col1++;
				}
				else if ((goal.x + goal.y) % 3 == 1) {
					goalcolor << setColor(2);
					col2++;
				}
				else {
					goalcolor << setColor(3);
					col3++;
				}
			}
		}
	}

	void update() override
	{
		for (auto it1 = goals.begin(); it1 != goals.end();)
		{
			if (player1.intersects(*it1)) {
				for (int i = 0; i < goals.size(); i++) {
					if (*it1 == goals[i]) {
						for (auto it2 = goalcolor.begin(); it2 != goalcolor.end();) {
							if (goalColorNum == i) {
								if (backcolor == goalcolor[i]) {
									gamefinish = true;
									it1 = goals.erase(it1);
									it2 = goalcolor.erase(it2);
									goalColorNum = 0;
									break;
								}
								else {
									timer = Timer{ getData().timeSet, StartImmediately::Yes };
									backcolor = goalcolor[i];
									it1 = goals.erase(it1);
									it2 = goalcolor.erase(it2);
									goalColorNum = 0;
									getData().score += 10;
									break;
								}
							}
							goalColorNum++;
							++it2;
						}
						break;
					}
				}
			}
			else {
				++it1;
			}
		}

		if (goals.size() == 0) {
			clear = true;
			gamefinish = true;
		}
		if (timer.reachedZero()) {
			gamefinish = true;
		}
		if (gamefinish == true)
		{
			getData().prescore= getData().score;
			getData().highscore = Max(getData().highscore, getData().score);
			if (clear == true) {
				changeScene(U"GameClear", 5.0s);
			}
			else {
				changeScene(U"GameOver", 2.0s);
			}
			
		}
		const double deltaTime = Scene::DeltaTime();
		const Vec2 move = Vec2(KeyRight.pressed() - KeyLeft.pressed(), KeyDown.pressed() - KeyUp.pressed())
			.setLength(deltaTime * playerSpeed * (KeyShift.pressed() ? 0.5 : 1.0));
		player1.moveBy(move);
		if (KeyZ.down()) {
			Console << goals;
		}
	}
	void draw() const override
	{
		Scene::SetBackground(backcolor);
		RectF{ 600,0, 200, 600 - 500 * timer.progress0_1() }.draw(Palette::White);
		Line(600, 100, 800, 100).draw(Palette::Red);
		RectF{ 600,600 - 500 * timer.progress0_1(), 200, 500 * timer.progress0_1() }.draw(Palette::Black);
		for (auto i : step(goals.size())) {
			goals[i].draw(goalcolor[i]);
		}
		player1.drawFrame(1, 1, Palette::Black);
		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(640, 35, Palette::Black);
	}

	void drawFadeOut(double t) const override
	{
		draw();
		if (clear == false) {
			Circle{ player1.x,player1.y, t * 1000 }.draw(Palette::Black);
		}
		else {
			Circle{ player1.x,player1.y, t * 1000 }.draw(Palette::White);
		}
	}
};
class GameClear : public App::Scene {
public:
	GameClear(const InitData& init) :IScene(init) {
	}
	void update() override {
		if (MouseR.down())
		{
			if (getData().stageNum < getData().allStage) {
				getData().stageNum++;
				getData().allStage = getData().num * 2 + 1;
				getData().ballnum = getData().stageNum * getData().allStage + 1;
				changeScene(U"Game");
			}
			else {
				getData().stageNum = 1;
				changeScene(U"Title");
			}
		}
	}
	void draw() const override {
		Scene::SetBackground(Palette::White);
	}
};
class GameOver : public App::Scene {
public:
	GameOver(const InitData& init) :IScene(init) {
	}
	void update() override {
		if (MouseR.down())
		{
			getData().stageNum = 1;
			changeScene(U"Title");
		}
	}
	void draw() const override {
		Scene::SetBackground(Palette::Black);
	}
};
void Main()
{
	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);
	FontAsset::Register(U"ScoreFont", 30, Typeface::Bold);
	// シーンマネージャーを作成
	App manager;

	// タイトルシーン（名前は U"Title"）を登録
	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");
	manager.add<GameClear>(U"GameClear");
	manager.add<GameOver>(U"GameOver");
	//manager.init(U"Game");

	while (System::Update())
	{
		// 現在のシーンを実行
		if (!manager.update())
		{
			break;
		}
	}
}
