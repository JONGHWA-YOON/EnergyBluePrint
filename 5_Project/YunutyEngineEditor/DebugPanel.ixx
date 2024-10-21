/// 디버그 로그를 찍기 위한 페널
/// 오브젝트에 대한 클릭, 호버링 이벤트에 대한 디버그 값을 로그로 기록
/// 2023. 08. 23. 윤종화

#include <vector>;
#include <string>;
#include <mutex>
import ui.EditorPanel;
export module ui.DebugPanel;

export class DebugPanel
    : public EditorPanel
{
public:
    static std::mutex logMutex;
    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void OnUpdate(float ts) override;
    virtual void DrawPanel() override;

public:
    static void LogMessage(const char* text)
    {
        std::scoped_lock lock(logMutex);
        logs.push_back(text);
    }
    void ClearLogs() {
        logs.clear();
    }
private:
    static std::vector<std::string> logs;
};
