#include <array>
#include "appwindow.h"
#include "slint.h"
#include "platform.h"
#include <KDGui/gui_application.h>

static bool windowVisible = true;

/// Create a custom platform which serves to wrap the WindowAdapter and return
/// it when slint queries it internally. Register said platform with slint.
slint::ComponentHandle<AppWindow> connectSlintToApp()
{
	auto platform = std::make_unique<KDSlintPlatform>();
	auto windowAdapter = std::make_unique<LinuxWindowAdapter>();
	auto &window = windowAdapter->kdGuiWindow();

	// window settings need to be set here (they are made read-only later I
	// think?)
	window.title = "Slint/KDGui Integration";
	window.width = 720;
	window.height = 480;
	window.visible = true;

	// quit the internal app on window close
	window.visible.valueChanged().connect([](bool visible) {
		windowVisible = visible;
		if (!visible) {
			KDGui::GuiApplication::instance()->quit();
		}
	});

	platform->m_windowAdapter = std::move(windowAdapter);
	slint_platform::Platform::register_platform(std::move(platform));

	auto gui = AppWindow::create();

	gui->on_request_increase_value(
		[&] { gui->set_counter(gui->get_counter() + 1); });

	gui->show();

	return std::move(gui);
}

auto main() -> int
{
	KDGui::GuiApplication app;
	auto gui = connectSlintToApp();

	while (windowVisible) {
		app.processEvents();
	}

	return app.exec();
}
