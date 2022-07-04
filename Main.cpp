# include <Siv3D.hpp> // OpenSiv3D v0.6.3

struct GameData
{
	int32 score = 0, highscore = 0;
	int32 num = 0, ballnum = 10;
};
using App = SceneManager<String, GameData>;
class Title :public App::Scene {
public:
	Title(const InitData& init) :IScene(init){
	}
	void update() FMT_OVERRIDE {
		
		if (MouseR.down())
		{
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
		polygon1.draw(Palette::Skyblue);
		polygon2.draw(Palette::Skyblue);
		if (polygon1.mouseOver() )//Right
		{
			polygon1.draw(Palette::White);
			if (MouseL.down())
			{
				getData().num++;

			}
		}
		else
		{
			polygon1.draw(Palette::Skyblue);
		}
		if (polygon2.mouseOver() && getData().num >= 1)//Left
		{
			polygon2.draw(Palette::White);
			if (MouseL.down())
			{
				getData().num--;

			}
		}
		else
		{
			polygon2.draw(Palette::Skyblue);
		}
		Scene::SetBackground(ColorF(0.3, 0.4, 0.5));
		FontAsset(U"TitleFont")(U"Color Game").drawAt(400, 100);
		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(590, 490);
		FontAsset(U"ScoreFont")(U"HighScore: {}"_fmt(getData().highscore)).draw(520, 540);

	}
};
class Game : public App::Scene {
private:
	ColorF backcolor = Palette::Green;
	bool gameover = false;
	Circle player1{ 400, 500, 10 };
	Array<Rect>goals;
	Array<ColorF>goalcolor;
	int32 n = 0;
	Vec2 RandomGoal()
	{
		return RandomVec2({ 50, 750 }, { 50, 450 });
	}
	static constexpr double playerSpeed = 550.0;
public:
	Game(const InitData& init) : IScene(init) {
		for (auto i : step(getData().ballnum)) {
			goals << Rect(RandomGoal().x, RandomGoal().y, 20, 20);
		}
		for (const auto& goal : goals)
		{

			if ((goal.x + goal.y) % 3 == 0) {
				goalcolor << Palette::Aliceblue;
			}
			else if ((goal.x + goal.y) % 3 == 1) {
				goalcolor << Palette::Antiquewhite;
			}
			else {
				goalcolor << Palette::Burlywood;
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
							if (n == i) {
								backcolor = goalcolor[i];
								it1 = goals.erase(it1);
								it2 = goalcolor.erase(it2);
								n = 0;
								break;
							}
							n++;
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
			gameover = true;
		}
		if (gameover==true)
		{
			changeScene(U"Title");
		}
		const double deltaTime = Scene::DeltaTime();
		const Vec2 move = Vec2(KeyRight.pressed() - KeyLeft.pressed(), KeyDown.pressed() - KeyUp.pressed())
			.setLength(deltaTime * playerSpeed * (KeyShift.pressed() ? 0.5 : 1.0));
		player1.moveBy(move);
		if (KeyZ.down()) {
			Console << goalcolor;
			Console << goals;
		}
		getData().score += static_cast<int32>(Cursor::Delta().length() * 10);
	}
	void draw() const override
	{
		Scene::SetBackground(backcolor);
		for (auto i: step (goals.size())) {
			goals[i].draw(goalcolor[i]);
		}
		player1.drawFrame(1, 1, Palette::Black);
		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).draw(40, 40, Palette::Black);
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

	manager.init(U"Game");

	while (System::Update())
	{
		// 現在のシーンを実行
		if (!manager.update())
		{
			break;
		}
	}
}
