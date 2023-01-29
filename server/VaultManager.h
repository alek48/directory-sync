#pragma once

#include <vector>
#include <string>

class Vault;

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
    bool isCreated(const std::string& name);
    
    bool createVault(const std::string& name);
    bool openVault(const std::string& name);
    bool closeVault(const std::string& name);

    void setStoragePath(const std::string& storagePath);
    std::string getStoragePath() const;

    Vault& getVault(const std::string& name);

    friend class Vault;

private:
    std::vector<Vault>::iterator findVault(const std::string& name);
    bool isOpened(const std::string& name);
    std::string storagePath;
    std::vector<Vault> openVaults;
};