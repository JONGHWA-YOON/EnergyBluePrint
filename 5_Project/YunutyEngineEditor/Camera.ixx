/// 2023. 07. 09. 김상준
/// 에디터에서 사용할 카메라 클래스
/// 렌더러에서 사용하는 카메라와 양식을 같게 함
/// 
/// 인터페이스는 아니지만,
/// 해당 카메라를 상속받아서 구체화된 카메라를 만들 수 있음
export module graphics.Camera;

export namespace graphics
{
    class Camera
    {
    public:
        virtual void Init() = 0;
        virtual void Shutdown() = 0;
        virtual void OnUpdate(float ts) = 0;

    private:
        // 4x4 매트릭스 필요할 듯
    };
}