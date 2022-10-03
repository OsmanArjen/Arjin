#include "../include/Pool.hpp"
#include "iostream"
int main()
{
/*	GameMain game;

	game.run();*/

	Pool<int> cpool{10};
	
	for(int i : {0,1,2,3,4,5,6,7,8,9})
		cpool.insert(i, (i*(i+i))/(i+1));

	for(auto [id, data] : cpool.each())
	{
		std::cout << id << ":" << data << '\n';
	}

	return 0;
}


