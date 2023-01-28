#pragma once

#include "Vault.h"

#include <vector>

class VaultManager
{
public:
    VaultManager(std::string storagePath);

    std::vector<std::string> listAllVaults();

    void createVault(std::string name);
    void openVault(std::string name);
    void closeVault(std::string name);

private:
    std::string storagePath;
    std::vector<Vault> openVaults;
};