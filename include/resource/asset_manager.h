#pragma once

#include <memory>
#include "utils/json.h"

namespace zidian{

    //写入字符串到文件
    inline void WriteStringToFile(const char *path , std::wstring content){
        std::ofstream file(path);
        file << ToByteString(content);
        file.close();
    }

    //二进制方式读取文件 
    std::unique_ptr<uint8_t[]> ReadFileAsBinary(std::string &path , long &length);
    
    //从任意路径读取文本文件
    std::wstring ReadFileAsWstring(std::string path);

    std::string ReadFileAsString(std::string path);

    class AssetManager{
    public:
        AssetManager();

        virtual ~AssetManager(); 

        static std::unique_ptr<AssetManager>& getInstance();

        std::string assetDir();

        std::string shaderDir();

        virtual std::wstring readAssetFileAsText(std::string path);

        virtual std::string readAssetFileAsString(std::string path);

        virtual std::vector<char> readShaderFileAsBinary(std::string path, long &length);

        virtual std::unique_ptr<uint8_t[]> readAssetFileAsBinary(std::string path, long &length);

        virtual uint8_t* loadImageFileFromPath(std::string path, int &width, int &height,
                            int &channel, bool need_flip = false);

        virtual uint8_t* readAssetImageFile(std::string path, int &width, int &height,
                            int &channel, bool need_flip = false);
    private:
        static std::unique_ptr<AssetManager> m_instance;

        std::string rootDir();
    };
}