#pragma once

namespace DC
{
	// Private implementation class which holds all Vulkan objects etc and has various methods which we keep in here
	// instead of in the Renderer class, to keep that class nice and tidy.
	class Renderer::Pimpl
	{
	public:
		Pimpl();

		// Called from Renderer::init()
		void init(const Settings& settings);

		// Called from Renderer::update()
		// Returns false if the window/application has been asked to close/shutdown
		bool update(void);

		// Called from Renderer::shutdown()
		void shutdown(void);

	private:
		// Called from init to initialise Vulkan
		void initVulkan(void);

		// Called from init to initialise the application's window
		void initWindow(const Settings& settings);
	};

	Renderer::Pimpl::Pimpl()
	{
	}

	void Renderer::Pimpl::init(const Settings& settings)
	{
		initWindow(settings);
		initVulkan();

	}

	void Renderer::Pimpl::initWindow(const Settings& settings)
	{

	}

	void Renderer::Pimpl::initVulkan(void)
	{

	}

	bool Renderer::Pimpl::update(void)
	{
		// Return false if window has been asked to close.
		return false;
	}

	void Renderer::Pimpl::shutdown(void)
	{

	}
}