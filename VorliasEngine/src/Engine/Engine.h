namespace ENGINE_NS {
	class Application;
	class Engine {
	public:
		static Engine& GetInstance();
		void Run(Application* app);

	private:
		static Engine* s_instance;
		Engine();

        bool Initialize();
		void Update();
		void Render();
        void Shutdown();

		Application* m_app;

		bool m_isRunning = false;
		bool m_isInitialized = false;
	};
} // namespace ENGINE_NS