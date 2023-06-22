#include "AssetLoader.hpp"

#include <stdexcept>

#ifndef _WIN32

// XXX: I know this is a raw c header, needed for glob operations because it's not in the stl
extern "C" {
    #include <glob.h>
}

#endif

AssetLoader::AssetLoader()
    :
    m_assets()
{
    // 
}

void AssetLoader::Load(std::string const& expr, Shader const& shader, Shader const& skeletonShader)
{
#ifdef _WIN32
    WIN32_FIND_DATA fileFindData = {};
    HANDLE fileFindHandle = FindFirstFile(expr.c_str(), &fileFindData);

    if (fileFindHandle == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Failed to find files using pattern");

    do
    {
        std::string dir = expr.substr(0, expr.find_last_of('/'));
        std::string path = dir + "/" + fileFindData.cFileName;

        Asset* pAsset = new Asset{
            std::string(fileFindData.cFileName),
            std::unique_ptr<Mesh>(new Mesh(path, shader, skeletonShader))
        };

        m_assets.push_back(
            std::unique_ptr<Asset>(pAsset)
        );
    } while (FindNextFile(fileFindHandle, &fileFindData));

    FindClose(fileFindHandle);
#else
    glob_t globResult = {};
    int ret = glob(expr.c_str(), GLOB_TILDE, NULL, &globResult);
    if (ret != 0)
    {
        globfree(&globResult);
        throw std::runtime_error("Failed to glob input expression");
    }

    for (size_t i = 0; i < globResult.gl_pathc; i++)
    {
        Asset* pAsset = new Asset{
            std::string(globResult.gl_pathv[i]),
            std::unique_ptr<Mesh>(new Mesh(globResult.gl_pathv[i], shader))
        };

        m_assets.push_back(
            std::unique_ptr<Asset>(pAsset)
        );
    }

    globfree(&globResult);
#endif
}
