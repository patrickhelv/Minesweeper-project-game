#include "MinesweeperWindow.h"
#include <thread>

int main()
{

	Fl::background(200, 200, 200);
	constexpr int width = 10;
	constexpr int height = 10;
	constexpr int mines = 3;

	Point startPoint{ 200,300 };
	MinesweeperWindow mw{ startPoint, width, height, mines, "Minesweeper" };
	thread checkwincondition(&MinesweeperWindow::win, &mw);
 	gui_main();
	checkwincondition.join();
	return 0;

}
 