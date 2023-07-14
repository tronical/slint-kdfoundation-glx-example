#include "window_adapter.h"
#include <KDGui/gui_application.h>

// linux specific stuff
#include <KDGui/platform/linux/xcb/linux_xcb_platform_integration.h>
#include <KDGui/platform/linux/wayland/linux_wayland_platform_integration.h>
#include <xcb/xproto.h>

namespace slint_platform = slint::experimental::platform;

LinuxWindowAdapter::LinuxWindowAdapter() noexcept
{
	// KDGui::Window uses GuiApplication::instance and gets its platform
	// integration.
	m_kdWindow = std::make_unique<KDGui::Window>();
	m_kdWindow->create();

	auto slintSize = physical_size();

	auto *platformIntegration =
		KDGui::GuiApplication::instance()->guiPlatformIntegration();

	if (auto *xcbLinuxPlatformIntegration =
			dynamic_cast<KDGui::LinuxXcbPlatformIntegration *>(
				platformIntegration)) {
		auto *xcbPlatformWindow = dynamic_cast<KDGui::LinuxXcbPlatformWindow *>(
			m_kdWindow->platformWindow());

		auto *xcbConnection = xcbLinuxPlatformIntegration->connection();
		auto *xcbScreen = xcbLinuxPlatformIntegration->screen();
		auto xcbWindowHandle = xcbPlatformWindow->handle();

		// getting the visual_id of the window requires xcb interface
		xcb_visualid_t xcbVisual;
		{
			auto windowAttributesCookie =
				xcb_get_window_attributes(xcbConnection, xcbWindowHandle);
			auto errorResponse = std::make_unique<xcb_generic_error_t>();
			// evil pointer cast...
			auto *windowAttributesReply = xcb_get_window_attributes_reply(
				xcbConnection, windowAttributesCookie,
				(xcb_generic_error_t **)&errorResponse);

			if (windowAttributesReply == nullptr) {
				xcbLinuxPlatformIntegration->logger()->critical(
					"X11 Error {}", errorResponse->error_code);
				std::abort();
			}
			xcbVisual = windowAttributesReply->visual;
		}

		m_renderer.emplace(slint_platform::NativeWindowHandle::from_x11_xcb(
							   xcbWindowHandle, xcbVisual, xcbConnection, 0),
						   slintSize);
	} else if (auto *waylandLinuxPlatformIntegration =
				   dynamic_cast<KDGui::LinuxWaylandPlatformIntegration *>(
					   platformIntegration)) {
		auto *waylandPlatformWindow =
			dynamic_cast<KDGui::LinuxWaylandPlatformWindow *>(
				m_kdWindow->platformWindow());

		auto *display = waylandLinuxPlatformIntegration->display();
		auto *surface = waylandPlatformWindow->handle();

		m_renderer.emplace(
			slint_platform::NativeWindowHandle::from_wayland(surface, display),
			slintSize);
	}

	// attach a resize handler to the KDGui window dimensions, use it to
	// notify slint that we have resized
	auto resize_handler = [this]() {
		auto size = physical_size();
		this->resize(size.width, size.height);
		spdlog::log(spdlog::level::debug, "Resized window");
	};
	m_kdWindow->width.valueChanged().connect(resize_handler);
	m_kdWindow->height.valueChanged().connect(resize_handler);
}

void LinuxWindowAdapter::show() const { m_renderer->show(); }

void LinuxWindowAdapter::hide() const { m_renderer->hide(); }

void LinuxWindowAdapter::request_redraw() const {}

void LinuxWindowAdapter ::render() const
{
	m_renderer->render(window());
	if (has_active_animations())
		request_redraw();
}

void LinuxWindowAdapter::resize(uint32_t width, uint32_t height)
{
	slint::PhysicalSize windowSize({width, height});
	m_renderer->resize(windowSize);
	dispatch_resize_event(slint::LogicalSize({(float)width, (float)height}));
}
