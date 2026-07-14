#include "renderer/render.h"
#include "renderer/shader/shader_manager.h"
#include "resource/asset_manager.h"
#include "utils/log.h"

namespace zidian {
    ShaderManager::ShaderManager(Render &context) : ctx(context){
    }

    VkShaderModule ShaderManager::createShaderModule(const std::string& path){
        if(shaderModuleMap.find(path) != shaderModuleMap.end()){
            return shaderModuleMap[path];
        }


        long codeSize = 0;
        auto code = AssetManager::getInstance()->readShaderFileAsBinary(path, codeSize);
        if(codeSize <= 0){
            Log::e("shader_manager","failed to open shader file: %s", path.c_str());
            return {};
        }

        Log::i("shader_manager","read shader file: %s size = %ld", path.c_str(), codeSize);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        
        VkShaderModule shaderModule;
        if(vkCreateShaderModule(ctx.device,&createInfo,nullptr,&shaderModule) != VK_SUCCESS) {
            Log::e("shader_manager","create shader module error");
            return {};
        }

        shaderModuleMap[path] = shaderModule;
        return shaderModule;
    }
        
    void ShaderManager::destroyShaderModule(VkShaderModule shaderModule){
        vkDestroyShaderModule(ctx.device, shaderModule , nullptr);
        removeShaderModuleFromMap(shaderModule);
    }

    void ShaderManager::clearAllShaderModules(){
        for(auto &p : shaderModuleMap){
            vkDestroyShaderModule(ctx.device, p.second, nullptr);
        }//end for each
        shaderModuleMap.clear();
    }

    bool ShaderManager::removeShaderModuleFromMap(VkShaderModule &module){
        std::string key;
        for(auto &p : shaderModuleMap){
            if(p.second == module){
                key = p.first;
                break;
            }
        }
        if(!key.empty()){
            return shaderModuleMap.erase(key) > 0;
        }
        return false;
    }
}