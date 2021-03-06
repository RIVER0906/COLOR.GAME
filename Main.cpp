# include <Siv3D.hpp> // OpenSiv3D v0.6.3

struct GameData
{
	//スコア関連
	int32 score = 0, highscore = 0, prescore = 0;
	//タイトルで設定する難易度、ブロックの数
	int32 num = 0, ballnum = 10;
	//ステージの数
	int32 stageNum = 1, allStage = 1;
	//制限時間
	SecondsF timeSet = 0s;
	//調整弁
	bool adjust = true;
};
using App = SceneManager<String, GameData>;
//タイトル画面
class Title :public App::Scene {
private:
	//スタートのボタン
	Rect Start{ 325, 460, 150, 80 };
public:
	Title(const InitData& init) :IScene(init) {
	}
	void update() FMT_OVERRIDE {
		//ゲームスタート,中身は難易度別に変化
		if (MouseL.down() && Start.mouseOver())
		{
			getData().stageNum = 1;
			getData().score = 0;
			if (getData().num == 0) {
				getData().timeSet = 30.0s;
				getData().allStage = 2;
				getData().ballnum = getData().stageNum * getData().allStage + 1;
			}
			else if (getData().num == 1) {
				getData().timeSet = 15.0s;
				getData().allStage = 3;
				getData().ballnum = getData().stageNum * getData().allStage + 1;
			}
			else if (getData().num == 2) {
				getData().timeSet = 10.0s;
				getData().allStage = 4;
				getData().ballnum = getData().stageNum * 10;
			}
			else if (getData().num == 3) {
				getData().timeSet = 5.0s;
				getData().allStage = 4;
				getData().ballnum = getData().stageNum * 5;
			}
			else {
				getData().timeSet = 15.0s;
				getData().ballnum = Random(30,50);
			}
			changeScene(U"Game");
		}

	}
	void draw() const override
	{
		//＞図形、難易度アップ
		const Polygon polygon1
		{
			Vec2{ 550, 225 }, Vec2{ 600, 300 }, Vec2{ 550, 375 }, Vec2{ 575, 300 }
		};
		//＜の図形、難易度ダウン
		const Polygon polygon2
		{
			Vec2{ 250, 225 }, Vec2{ 225, 300 }, Vec2{ 250, 375 }, Vec2{ 200, 300 }
		};
		polygon1.draw(Palette::Blue);
		polygon2.draw(Palette::Blue);
		if (polygon1.mouseOver() && getData().num <= 3)//Right
		{
			polygon1.draw(Palette::Aqua);
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
			polygon2.draw(Palette::Aqua);
			if (MouseL.down())
			{
				getData().num--;

			}
		}
		else
		{
			polygon2.draw(Palette::Skyblue);
		}
		//背景
		Scene::SetBackground(Palette::Darkslateblue);
		//タイトル
		FontAsset(U"TitleFont")(U"Color Game").drawAt(400, 100);
		//前回のスコア・ハイスコア
		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().prescore)).draw(590, 490);
		FontAsset(U"ScoreFont")(U"HighScore: {}"_fmt(getData().highscore)).draw(520, 540);
		if (getData().num == 0) {
			FontAsset(U"ScoreFont")(U"EASY").drawAt(400, 300);
		}
		if (getData().num == 1) {
			FontAsset(U"ScoreFont")(U"NORMAL").drawAt(400, 300);
		}
		if (getData().num == 2) {
			FontAsset(U"ScoreFont")(U"HARD").drawAt(400, 300);
		}
		if (getData().num == 3) {
			FontAsset(U"ScoreFont")(U"CHALLENGE").drawAt(400, 300);
		}
		if (getData().num == 4) {
			FontAsset(U"ScoreFont")(U"INFINITY").drawAt(400, 300);
		}
		if (Start.mouseOver()) {
			Start.rounded(10).draw(Palette::Aqua);
		}
		else {
			Start.rounded(10).draw(Palette::Skyblue);
		}
		FontAsset(U"ScoreFont")(U"START").drawAt(400, 500);
	}
};
//ゲーム画面
class Game : public App::Scene {
private:
	//制限時間
	Timer timer{ getData().timeSet, StartImmediately::Yes };
	//背景の色
	ColorF backcolor = Palette::Burlywood;
	//終了判定
	bool gamefinish = false;
	//クリア判定：終了判定とは別に、これがないとバグるため重要
	bool clear = false;
	//自機
	Vec2 player0{ 300, 500 };
	Circle player1{ 300, 500, 10 };
	//ブロック関連のデータ
	Array<Rect>goals;
	Array<ColorF>goalcolor;
	int32 goalColorNum = 0;
	Array<int32>goalX, goalY;
	int32 col1 = 0, col2 = 0, col3 = 0;
	ColorF setColor(int x) {
		ColorF color;
		if (x == 1)color = Palette::Lemonchiffon;
		if (x == 2)color = Palette::Aqua;
		if (x == 3)color = Palette::Violet;
		return color;
	}
	Vec2 RandomGoal()
	{
		return RandomVec2({ 1, 579 }, { 1, 579 });
	}
	//ブロック同士、またはブロックと自機が被らないようにする
	bool allowance = false;
	//エフェクト
	Effect effect;
	//自機のスピード
	static constexpr double playerSpeed = 300.0;
public:
	Game(const InitData& init) : IScene(init) {
		//ブロック配置、ランダム且つ被らないように調整済み
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
		//ブロックの色、勿論ランダムだが、詰まないように前半のif文で調整
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
		//ブロックと自機の接触判定、ブロック消去時のエフェクト
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
									effect.add([posX = goals[i].x, posY = goals[i].y,color=backcolor](double t)
									{
										const double t2 = (1.0 - t);
										Circle{ posX,posY, 10 - t * 70 }.drawFrame(20 * t2, AlphaF(t2 * 0.5), color);
										return (t < 1.0);
									});
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
		//ゲームの終了条件
		if (gamefinish == true)
		{
			getData().adjust = false;
			getData().prescore= getData().score;
			getData().highscore = Max(getData().highscore, getData().score);
			if (clear == true) {
				changeScene(U"GameClear", 4.0s);
			}
			else {
				changeScene(U"GameOver", 2.0s);
			}
			
		}
		if (goals.size() == 0 && gamefinish == false) {
			clear = true;
			gamefinish = true;
		}
		if (timer.reachedZero()) {
			gamefinish = true;
		}
		//自機の動き
		const double deltaTime = Scene::DeltaTime();
		const Vec2 move = Vec2(KeyRight.pressed() - KeyLeft.pressed(), KeyDown.pressed() - KeyUp.pressed())
			.setLength(deltaTime * playerSpeed * (KeyShift.pressed() ? 0.5 : 1.0));
		player0.moveBy(move).clamp(Rect(10,10,580,580));
		player1.x = player0.x;
		player1.y = player0.y;
		/*バグ修正時に使用
		if (KeyZ.down()) {
			Console << goals;
		}
		*/
	}
	void draw() const override
	{
		//描画//
		//背景
		Scene::SetBackground(backcolor);
		//制限時間
		RectF{ 600,0, 200, 600 - 500 * timer.progress0_1() }.draw(Palette::White);
		Line(600, 100, 800, 100).draw(Palette::Red);
		RectF{ 600,600 - 500 * timer.progress0_1(), 200, 500 * timer.progress0_1() }.draw(Palette::Black);
		//ブロック
		for (auto i : step(goals.size())) {
			goals[i].draw(goalcolor[i]);
		}
		//自機
		player1.drawFrame(1, 1, Palette::Black);
		//スコア
		FontAsset(U"ScoreFont")(U"Score: {}"_fmt(getData().score)).drawAt(700, 50, Palette::Black);
		//エフェクト
		effect.update();
	}

	void drawFadeOut(double t) const override
	{
		//クリア、失敗でフェードアウト時の挙動に変化
		draw();
		if (clear == false) {
			Circle{ player1.x,player1.y, t * 1000 }.draw(Palette::Black);
		}
		else {
			Circle{ player1.x,player1.y, t * 2000 }.drawFrame(0, 5, Palette::White);
			Circle{ player1.x,player1.y, t * 1500 }.drawFrame(0,10,Palette::White);
			Circle{ player1.x,player1.y, t * 1000 }.draw(ColorF(1.0, 1.0, 1.0, 0.75 + t * 0.25));
		}
	}
};
//クリア画面
class GameClear : public App::Scene {
private:
	//NEXTボタン
	Rect Next{ 325, 460, 150, 80 };
public:
	GameClear(const InitData& init) :IScene(init) {
	}
	void update() override {
		//次に進むための条件、難易度やステージで変化
		if (MouseL.down() && Next.mouseOver())
		{
			if (getData().num != 4) {
				if (getData().stageNum < getData().allStage) {
					getData().adjust = true;
					getData().stageNum++;
					if (getData().num == 0) {
						getData().ballnum = getData().stageNum * getData().allStage + 1;
					}
					else if (getData().num == 1) {
						getData().ballnum = getData().stageNum * getData().allStage + 1;
					}
					else if (getData().num == 2) {
						getData().ballnum = getData().stageNum * 10;
					}
					else {
						getData().ballnum = getData().stageNum * 5;
					}
					changeScene(U"Game");
				}
				else {
					changeScene(U"Title");
				}
			}
			else {
				getData().stageNum++;
				getData().ballnum = Random(30, 50);
				changeScene(U"Game");
			}
		}
	}
	void draw() const override {
		//描画//
		//背景
		Scene::SetBackground(Palette::White);
		//難易度毎に表示内容変化
		if (getData().num != 4) {
			if (getData().stageNum < getData().allStage || getData().adjust == true) {
				FontAsset(U"OtherBFont")(U"Stage Clear").drawAt(400, 200, Palette::Black);
				FontAsset(U"OtherSFont")(U"{}"_fmt(getData().stageNum), U"/{}"_fmt(getData().allStage), U" Stage").drawAt(400, 300, Palette::Black);
			}
			else {
				FontAsset(U"OtherBFont")(U"Game Clear").drawAt(400, 200, Palette::Black);
				FontAsset(U"OtherSFont")(U"Last Score: {}"_fmt(getData().prescore)).drawAt(400, 300, Palette::Black);
			}
		}
		else {
			FontAsset(U"OtherBFont")(U"Stage Clear").drawAt(400, 200, Palette::Black);
			FontAsset(U"OtherSFont")(U"{}"_fmt(getData().stageNum), U" Stage").drawAt(400, 300, Palette::Black);
		}
		//NEXTボタン
		if (Next.mouseOver()) {
			Next.rounded(10).draw(Palette::Aqua);
		}
		else {
			Next.rounded(10).draw(Palette::Skyblue);
		}
		FontAsset(U"OtherSFont")(U"NEXT").drawAt(400, 500);
	}

};
//失敗画面
class GameOver : public App::Scene {
private:
	//左のボタン、同じ難易度で最初から
	Rect Retry{ 125, 460, 150, 80 };
	//右のボタン、タイトルに戻る
	Rect Finish{ 525, 460, 150, 80 };
public:
	GameOver(const InitData& init) :IScene(init) {
	}
	void update() override {
		//上二つのボタン操作、難易度毎に変化
		if (MouseL.down() && Finish.mouseOver())
		{
			getData().stageNum = 1;
			changeScene(U"Title");
		}
		if (MouseL.down() && Retry.mouseOver()) {
			getData().stageNum = 1;
			getData().score = 0;
			if (getData().num == 0) {
				getData().ballnum = getData().stageNum * getData().allStage + 1;
			}
			else if (getData().num == 1) {
				getData().ballnum = getData().stageNum * getData().allStage + 1;
			}
			else if (getData().num == 2) {
				getData().ballnum = getData().stageNum * 10;
			}
			else if (getData().num == 3) {
				getData().ballnum = getData().stageNum * 5;
			}
			else {
				getData().ballnum = Random(30, 50);
			}
			changeScene(U"Game");
		}
	}
	void draw() const override {
		//描画//
		//背景
		Scene::SetBackground(Palette::Black);
		//失敗の表示、スマホゲームの広告を意識
		Circle(400, 200, 60).drawFrame(0,10,Palette::White);
		FontAsset(U"OtherBFont")(U"失敗").drawAt(400, 200, Palette::White);
		//最終スコア
		FontAsset(U"OtherSFont")(U"Last Score: {}"_fmt(getData().prescore)).drawAt(400, 350, Palette::White);
		//retryとfinishのボタン
		if (Retry.mouseOver()) {
			Retry.rounded(10).draw(Palette::Orange);
		}
		else {
			Retry.rounded(10).draw(Palette::Skyblue);
		}
		FontAsset(U"OtherSFont")(U"RETRY").drawAt(200, 500);
		if (Finish.mouseOver()) {
			Finish.rounded(10).draw(Palette::Orange);
		}
		else {
			Finish.rounded(10).draw(Palette::Skyblue);
		}
		FontAsset(U"OtherSFont")(U"FINISH").drawAt(600, 500);
	}
};
//メイン
void Main()
{
	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);
	FontAsset::Register(U"ScoreFont", 30, Typeface::Bold);
	FontAsset::Register(U"OtherBFont", 50, Typeface::Bold);
	FontAsset::Register(U"OtherSFont", 25, Typeface::Bold);
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
