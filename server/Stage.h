#pragma once

#include <vector>
#include <string>

enum Stage
{
    Connected, Syncing
};

std::vector<std::string> getAvailableCommands(Stage stage);

std::string stageName(Stage stage);
