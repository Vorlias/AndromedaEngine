namespace ENGINE_NS {
    class Application {
    public:
        virtual bool Initialize() { return true; }
        virtual void Update() {}
        virtual void Render() {}
        virtual void Shutdown() {}

        virtual void ApplicationQuit() {}
    };
}
