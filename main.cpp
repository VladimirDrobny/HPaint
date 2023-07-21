#include "app.h"

int main(int argc, char *argv[])
{
	App app = App();
	app.setup();
	app.run();
	app.quit();
    return 0;
}
