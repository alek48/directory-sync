#include "VaultManager.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem; // requires c++17

VaultManager* VaultManager::vaultManager_ = nullptr;

VaultManager *VaultManager::getInstance()
{
    if(vaultManager_==nullptr){
        vaultManager_ = new VaultManager{};
    }
    return vaultManager_;
}

std::vector<std::string> VaultManager::listAllVaults()
{
    std::vector<std::string> vaults;

    for (const auto& entry : fs::directory_iterator(storagePath))
    {
        if (fs::is_directory(entry.status()))
            vaults.push_back(entry.path().filename());
    }

    return vaults;
}

void VaultManager::createVault(std::string name)
{
    
}

void VaultManager::setStoragePath(std::string storagePath)
{
    this->storagePath = storagePath;
}