# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include <queue>
#include <memory>
#include <thread>
#include <mutex>

struct probability {
	double LEFT = 20;
	double TOP = 100;
	double RIGHT = 20;
} PROB;

bool Dice(int size, int border) {
	static std::random_device rnd;
	static std::mt19937 mt(rnd());
	std::uniform_int_distribution<> dist(1, size);
	if (dist(mt) < border) {
		return true;
	}
	return false;
}

bool IntersectWithArray(const Line& line, const std::vector<Line>& argLines) {
	for (const auto& l : argLines) {
		if (line.end == l.end || line.end == l.begin) {
			return true;
		}
	}
	return false;
}

struct tree {
	static std::vector<Line> allBranches;
	static std::queue<Vec2> vertexes;
	static int count;

	const int MAXHEIGHT;
	const int MAXCOUNT;
	const int BRANCHDELTA;
	std::shared_ptr<tree> left;
	std::shared_ptr<tree> top;
	std::shared_ptr<tree> right;
	tree(Vec2 startPos);
	void Grow();
	void Write();
	void Reset(Vec2 startPos);
};

std::vector<Line> tree::allBranches{};
std::queue<Vec2> tree::vertexes{};
int tree::count = 0;

void tree::Reset(Vec2 startPos) {
	this->left.reset();
	this->right.reset();
	this->top.reset();
	this->allBranches.clear();
	this->count = 0;
	while (this->vertexes.size() > 0) {
		this->vertexes.pop();
	}
	this->vertexes.push(startPos);
}

tree::tree(Vec2 startPos) :
	MAXHEIGHT(20),
	BRANCHDELTA(10),
	MAXCOUNT(10000)
{
	this->count++;
	if (this->count > this->MAXCOUNT) return;
	this->vertexes.push(startPos);
}

void tree::Grow() {
	if (!vertexes.size()) return;

	const Line branchBase = Line(this->vertexes.front().x, this->vertexes.front().y, this->vertexes.front().x + this->BRANCHDELTA, this->vertexes.front().y);
	this->vertexes.pop();
	if (Dice(100, PROB.LEFT * 100)) {
		Line branch(branchBase);
		branch.end.y -= this->BRANCHDELTA;
		if (!IntersectWithArray(branch, this->allBranches)) {
			this->left = std::make_shared<tree>(branch.end);
			this->allBranches.push_back(branch.draw(Palette::Green));
		}
	}

	if (Dice(100, PROB.TOP * 100)) {
		Line branch(branchBase);

		if (!IntersectWithArray(branch, this->allBranches)) {
			this->top = std::make_shared<tree>(branch.end);
			this->allBranches.push_back(branch.draw(Palette::Red));
		}
	}

	if (Dice(100, PROB.RIGHT * 100)) {
		Line branch(branchBase);
		branch.end.y += this->BRANCHDELTA;
		if (!IntersectWithArray(branch, this->allBranches)) {
			this->left = std::make_shared<tree>(branch.end);
			this->allBranches.push_back(branch.draw(Palette::Red));
		}
	}
}

void tree::Write() {
	for (auto& l : this->allBranches) {
		l.draw(Palette::Black);
	}
}

void Main() {
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));
	tree tr(Vec2(0, 400));
	std::unique_ptr<std::thread> autoGrowThread;
	bool autoGrow = false;
	std::mutex mtx;
	while (System::Update()) {
		ClearPrint();
		SimpleGUI::Slider(PROB.LEFT, Vec2(50, 100));
		SimpleGUI::Slider(PROB.TOP, Vec2(200, 100));
		SimpleGUI::Slider(PROB.RIGHT, Vec2(350, 100));


		Print << tr.count;
		if (autoGrow) {
			tr.Grow();
		}
		if (SimpleGUI::Button(U"reset", Vec2(250, 50))) {
			tr.Reset(Vec2(0, 400));
		}
		if (SimpleGUI::Button(U"grow", Vec2(50, 50))) {
			tr.Grow();
		}
		if (SimpleGUI::Button(U"auto", Vec2(150, 50))) {
			autoGrow = !autoGrow;
		}
		tr.Write();
	}
}