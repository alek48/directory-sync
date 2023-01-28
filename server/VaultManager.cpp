#include "VaultManager.h"

#include <filesystem>
#include <string>

VaultManager::VaultManager(std::string storagePath) :
    storagePath {storagePath}
{

}

std::vector<std::string> VaultManager::listAllVaults()
{
    std::vector<std::string> vaults;

    // for (const auto& entry : std::filesystem::directory_iterator(storagePath))
    // {
    //     vaults.push_back(entry.path());
    // }

    return vaults;
}