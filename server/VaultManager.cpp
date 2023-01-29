#include "VaultManager.h"
#include "Vault.h"
#include "User.h"
#include "Logger.h"

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

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

bool VaultManager::isCreated(const std::string& name)
{
    for (const auto& vaultName : listAllVaults())
        if (vaultName == name)
            return true;

    return false;
}

bool VaultManager::createVault(const std::string& name)
{
    bool success = false;
    try
    {
        success = fs::create_directory(fs::path(storagePath + name));
    }
    catch (...)
    {
        Logger::LOG(ERROR, "Failure while creating directory: " + name);
    }
    return success;
}

void VaultManager::setStoragePath(const std::string& storagePath)
{
    storagePath.back() == '/' ? 
        this->storagePath = storagePath : this->storagePath = storagePath + '/';
}

std::string VaultManager::getStoragePath() const
{
    return storagePath;
}

Vault& VaultManager::getVault(const std::string& name)
{
    if (!openVault(name))
        throw std::exception{};

    return *findVault(name);
}

std::vector<Vault>::iterator VaultManager::findVault(const std::string& name)
{
    return std::find_if(openVaults.begin(), openVaults.end(), 
        [&name](const auto& vault){return vault.getName() == name;});
}

bool VaultManager::isOpened(const std::string& name)
{
    return findVault(name) != openVaults.end();
}

bool VaultManager::openVault(const std::string& name)
{
    if (isOpened(name))
        return true;
    
    if (!isCreated(name))
    {
        Logger::LOG(ERROR, "Vault not created");
        return false;
    }

    openVaults.push_back(Vault{name});
    return true;
}