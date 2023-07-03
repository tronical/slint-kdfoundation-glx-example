#include "slint_platform.h"
#include <KDGui/window.h>

namespace slint_platform = slint::experimental::platform;

class LinuxWindowAdapter : public slint_platform::WindowAdapter
{
  public:
	LinuxWindowAdapter() noexcept;
	auto inline kdGuiWindow() const -> KDGui::Window & { return *m_kdWindow; }
	auto inline renderer() -> slint_platform::SkiaRenderer & override
	{
		return m_renderer.value();
	}
	inline auto physical_size() const -> slint::PhysicalSize final
	{
		return slint::PhysicalSize(
			{m_kdWindow->width.get(), m_kdWindow->height.get()});
	}

	void show() const override;
	void hide() const override;
	void request_redraw() const override;

	void render() const;
	void resize(uint32_t width, uint32_t height);

  private:
	std::optional<slint_platform::SkiaRenderer> m_renderer;
	std::unique_ptr<KDGui::Window> m_kdWindow;
};
