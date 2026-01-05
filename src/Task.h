#pragma once

/* -========================================================
    Includes
   ========================================================- */

#include "Utils.h"

/* -========================================================
    Types
   ========================================================- */

using PickTaskRet = geode::Result<std::filesystem::path>;
using PickManyTaskRet = geode::Result<std::vector<std::filesystem::path>>;
using PickTask = geode::Task<PickTaskRet>;
using PickManyTask = geode::Task<PickManyTaskRet>;

/* -========================================================
    Hooks
   ========================================================- */

PickTask pickHook(file::PickMode mode, const file::FilePickOptions & options);
PickManyTask pickManyHook(const file::FilePickOptions & options);
