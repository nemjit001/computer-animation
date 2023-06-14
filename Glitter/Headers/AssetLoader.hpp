#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"

#include <stdexcept>
// XXX: I know this is a raw c header, needed for glob operations because it's not in the stl
extern "C" {
    #include <glob.h>
}

struct Asset
{
    std::string m_name;
    std::unique_ptr<Mesh> m_mesh;
};

class AssetLoader
{
public:
    AssetLoader()
        :
        m_assets()
    {
        // 
    }

    void Load(std::string const& expr, Shader const& shader)
    {
        glob_t globResult = {};
        int ret = glob(expr.c_str(), GLOB_TILDE, NULL, &globResult);
        if (ret != 0)
        {
            globfree(&globResult);
            throw std::runtime_error("Failed to glob input expression");
        }

        for (size_t i = 0; i < globResult.gl_pathc; i++)
        {
            Asset* pAsset = new Asset {
                std::string(globResult.gl_pathv[i]),
                std::unique_ptr<Mesh>(new Mesh(globResult.gl_pathv[i], shader))            
            };

            m_assets.push_back(
                std::unique_ptr<Asset>(pAsset)
            );
        }

        globfree(&globResult);
    }

    std::vector<std::unique_ptr<Asset>> const& Get()
    {
        return m_assets;
    }

private:
    std::vector<std::unique_ptr<Asset>> m_assets;
};

