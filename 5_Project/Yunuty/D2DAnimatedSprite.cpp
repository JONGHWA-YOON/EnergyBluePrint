#include "YunuD2DGraphicCore.h"
#include "YunutyEngine.h"
#include <string>
#include <iostream>
#include <sys/types.h>
#include <filesystem>
#include <regex> 

unordered_map<wstring, SpriteAnim> D2DAnimatedSprite::cachedAnims;

void D2DAnimatedSprite::Play()
{
    index = 0;
    elapsed = 0;
}
void D2DAnimatedSprite::SetWidth(double width)
{
    this->width = width;
}
void D2DAnimatedSprite::SetHeight(double height)
{
    this->height = height;
}
void D2DAnimatedSprite::LoadAnimationFromFile(wstring folderName, double interval)
{
    if (loadedAnimFilePath != folderName)
        index = 0;

    loadedAnimFilePath = folderName;
    animSprites = LoadAnimation(folderName, interval);
}
void D2DAnimatedSprite::Update()
{
    if (!animSprites)
        return;

    if (animSprites->empty())
        return;

    if (index >= animSprites->size())
        return;

    elapsed += Time::GetDeltaTime();
    if ((*animSprites)[index].first < elapsed)
    {
        index++;
        if (index >= animSprites->size())
        {
            if (isRepeating)
            {
                index = 0;
                elapsed = 0;
            }
            else
            {
                index--;
            }
        }
    }
}
const SpriteAnim* D2DAnimatedSprite::LoadAnimation(wstring folderName, double interval)
{
    HANDLE dir;
    WIN32_FIND_DATA file_data;
    if (cachedAnims.find(folderName) != cachedAnims.end())
        return &cachedAnims[folderName];

    cachedAnims[folderName] = SpriteAnim();
    if ((dir = FindFirstFile((folderName + L"/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return nullptr; /* No files found */

    double time = 0;
    bool intervalFixed = interval > 0;
    do {
        const wstring file_name = file_data.cFileName;
        const wstring full_file_name = folderName + L"/" + file_name;
        if (file_name == L"." || file_name == L"..")
            continue;

        if (!intervalFixed)
        {
            wsmatch match;
            smatch match2;
            interval = 0;
            regex_match(file_name, match, wregex(L".*?(\\d+)ms.*"));
            //string a(file_name.begin(), file_name.end());
            //regex_match(a, match2, regex("\\D(\\d+)ms.*"));
            if (match.size() >= 2)
                interval = 0.001 * stoi(match[1]);
            if (interval <= 0)
                interval = 0.1;
        }
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (file_name[0] == '.')
            continue;

        if (is_directory)
            continue;

        cachedAnims[folderName].push_back(make_pair(time, full_file_name));
        time += interval;
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
    return &cachedAnims[folderName];
}
void D2DAnimatedSprite::Render(D2D1::Matrix3x2F transform)
{
    if (animSprites)
        if (!animSprites->empty())
            YunuD2D::YunuD2DGraphicCore::GetInstance()->DrawSprite((*animSprites)[index].second, transform, color, width, height);
}
const SpriteAnim* D2DAnimatedSprite::GetAnimSprites() const
{
    return animSprites;
}
void D2DAnimatedSprite::SetAnimation(const SpriteAnim* animation)
{
    animSprites = animation;
}
