
struct Rect
{
	int x;     int y;
	int width; int height;
};

struct Text
{
	int textid;
};


void acc(arjin::EntityType entt, Rect& rct, Text&, txt)
{
//...
}

int main()
{
	arjin::EntityWorld world{2};

	auto entities = world.create(12);

	for(auto entt : entities)
	{
		world.assign<Rect>(entt, 10,10, 10,10);
	}

	for(int x=0; x < 5; ++x)
	{
		world.assign<Text>(x, 0);
	}

	auto view = world.each<Rect, Text>();

	view.apply(render);

}