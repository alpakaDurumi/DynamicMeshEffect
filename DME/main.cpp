#include "App.h"

int main() {
	App app;

	if (!app.Initialize()) {
		MessageBox(nullptr, L"App Initialization Failed!", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	return app.Run();
}