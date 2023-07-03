#include "slint_platform.h"
#include "window_adapter.h"

namespace slint_platform = slint::experimental::platform;
class KDSlintPlatform : public slint_platform::Platform
{
  public:
	mutable std::unique_ptr<LinuxWindowAdapter> m_windowAdapter;

  private:
	[[nodiscard]] inline auto create_window_adapter() const
		-> std::unique_ptr<slint_platform::WindowAdapter> override
	{
		return std::move(m_windowAdapter);
	}
};
