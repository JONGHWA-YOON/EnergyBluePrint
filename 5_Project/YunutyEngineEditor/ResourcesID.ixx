module;
#include <string>
export module htkg.ResourcesID;
#define DebugMode
export namespace resourcesID
{
    // 일반 fbx를 불러오는건 시간이 오래 걸리기 때문에 debugPath를 따로 둔다.
#ifdef DebugMode
    inline const std::string FBXpath = "FBXFileDebug\\";
#else
    inline const std::string FBXpath = "FBXFile\\";
#endif
    inline const std::string ASEpath = "ASEFile\\";

#ifdef DebugMode
    inline const std::string TexturePath = "Textures\\";
#else
    inline const std::string TexturePath = "Textures\\";
#endif
    inline const std::string robotFBX = FBXpath + "Rob02.fbx";
    inline const std::string buttonFBX = FBXpath + "button.fbx";
    inline const std::string batteryFBX = FBXpath + "battery.fbx";
    //inline const std::string platformFBX = ASEpath + "Ground_Small_1.ase";
    inline const std::string platformFBX = FBXpath + "unitPlane.fbx";
    inline const std::string laserFBX = FBXpath + "laser.fbx";
    inline const std::string destinationFBX = FBXpath + "destination.fbx";

    inline const std::string redTexture = "Textures\\red.dds";
    inline const std::string orangeTexture = "Textures\\orange.dds";
    inline const std::string pinkTexture = "Textures\\pink.dds";
    inline const std::string purpleTexture = "Textures\\purple.dds";
    inline const std::string grayTexture = "Textures\\gray.dds";
    inline const std::string greenTexture = "Textures\\green.dds";
    inline const std::string cyanTexture = "Textures\\cyan.dds";
    inline const std::string scarletTexture = "Textures\\scarlet.dds";
    inline const std::string yellowTexture = "Textures\\yellow.dds";

    inline const std::string robotDiffuseTex = cyanTexture;
    inline const std::string buttonDiffuseTex = redTexture;
    inline const std::string batteryDiffuseTex = greenTexture;
    inline const std::string platformDiffuseTex = "Textures\\Ground_ALB_reverse.dds";
    inline const std::string platformNormalTex = "Textures\\Ground_NM_reverse.dds";
    inline const std::string laserDiffuseTex = yellowTexture;
    inline const std::string destinationDiffuseTex = orangeTexture;
}