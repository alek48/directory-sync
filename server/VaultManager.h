#pragma once

#include "Vault.h"

#include <vector>

class VaultManager // singleton
{
protected:
    VaultManager() = default;
    static VaultManager* vaultManager_;

public:
    VaultManager(VaultManager& other) = delete;
    void operator=(const VaultManager &) = delete;
    static VaultManager *getInstance();

    std::vector<std::string> listAllVaults();
    
    bool createVault(std::string name);
    bool openVault(std::string name);
    bool closeVault(std::string name);

    void setStoragePath(std::string storagePath);

private:
    std::string storagePath;
    std::vector<Vault> openVaults;
};