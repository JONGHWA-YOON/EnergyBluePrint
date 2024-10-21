/// 2023. 07. 09. 김상준
/// GameViewPanel에서 사용할 카메라 클래스
/// (Game 의 Main Camera)
/// 
/// 해당 패널에서는 하나의 카메라만을 사용할 것이므로,
/// 싱글턴 패턴으로 구현함
import graphics.Camera;
export module graphics.GameViewCamera;

export namespace graphics {
    class GameViewCamera : public Camera
    {
    public:
        static GameViewCamera* SingleInstance();
        static GameViewCamera& Get() { return *instance; }

        ~GameViewCamera();

        virtual void Init() override;
        virtual void Shutdown() override;
        virtual void OnUpdate(float ts) override;

    private:
        GameViewCamera();
        GameViewCamera(const GameViewCamera&) = delete;
        GameViewCamera& operator=(const GameViewCamera&) = delete;

        static GameViewCamera* instance;
    };
}
