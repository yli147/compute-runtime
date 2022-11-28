/*
 * Copyright (C) 2021-2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/helpers/compiler_hw_info_config.h"
#include "shared/test/common/fixtures/device_fixture.h"
#include "shared/test/common/helpers/debug_manager_state_restore.h"
#include "shared/test/common/helpers/unit_test_helper.h"
#include "shared/test/common/test_macros/hw_test.h"

using namespace NEO;

using CompilerHwInfoConfigFixture = Test<DeviceFixture>;

HWTEST_F(CompilerHwInfoConfigFixture, WhenIsMidThreadPreemptionIsSupportedIsCalledThenCorrectResultIsReturned) {
    auto &hwInfo = *pDevice->getRootDeviceEnvironment().getMutableHardwareInfo();
    UnitTestHelper<FamilyType>::setExtraMidThreadPreemptionFlag(hwInfo, false);
    auto compilerHwInfoConfig = CompilerHwInfoConfig::get(hwInfo.platform.eProductFamily);
    EXPECT_FALSE(compilerHwInfoConfig->isMidThreadPreemptionSupported(hwInfo));
    UnitTestHelper<FamilyType>::setExtraMidThreadPreemptionFlag(hwInfo, true);
    EXPECT_TRUE(compilerHwInfoConfig->isMidThreadPreemptionSupported(hwInfo));
}

using IsBeforeXeHpc = IsBeforeGfxCore<IGFX_XE_HPC_CORE>;

HWTEST2_F(CompilerHwInfoConfigFixture, GivenProductBeforeXeHpcWhenIsForceToStatelessRequiredThenFalseIsReturned, IsBeforeXeHpc) {
    auto &compilerHwInfoConfig = *CompilerHwInfoConfig::get(productFamily);
    EXPECT_FALSE(compilerHwInfoConfig.isForceToStatelessRequired());
}

using IsAtLeastXeHpc = IsAtLeastGfxCore<IGFX_XE_HPC_CORE>;

HWTEST2_F(CompilerHwInfoConfigFixture, GivenXeHpcAndLaterWhenIsForceToStatelessRequiredThenCorrectResultIsReturned, IsAtLeastXeHpc) {
    DebugManagerStateRestore restorer;
    auto &compilerHwInfoConfig = *CompilerHwInfoConfig::get(productFamily);
    EXPECT_TRUE(compilerHwInfoConfig.isForceToStatelessRequired());

    DebugManager.flags.DisableForceToStateless.set(false);
    EXPECT_TRUE(compilerHwInfoConfig.isForceToStatelessRequired());

    DebugManager.flags.DisableForceToStateless.set(true);
    EXPECT_FALSE(compilerHwInfoConfig.isForceToStatelessRequired());
}

HWTEST2_F(CompilerHwInfoConfigFixture, givenAotConfigWhenSetHwInfoRevisionIdThenCorrectValueIsSet, IsAtMostDg2) {
    auto hwInfo = *defaultHwInfo;
    auto &productHelper = getHelper<ProductHelper>();
    auto productConfig = productHelper.getProductConfigFromHwInfo(*defaultHwInfo);
    AheadOfTimeConfig aotConfig = {0};
    aotConfig.ProductConfig = productConfig;
    CompilerHwInfoConfig::get(hwInfo.platform.eProductFamily)->setProductConfigForHwInfo(hwInfo, aotConfig);
    EXPECT_EQ(hwInfo.platform.usRevId, aotConfig.ProductConfigID.Revision);
}

HWTEST2_F(CompilerHwInfoConfigFixture, givenAtMostXeHPWhenGetCachingPolicyOptionsThenReturnNullptr, IsAtMostXeHpCore) {
    auto compilerHwInfoConfig = CompilerHwInfoConfig::get(defaultHwInfo->platform.eProductFamily);
    EXPECT_EQ(compilerHwInfoConfig->getCachingPolicyOptions(false), nullptr);
    EXPECT_EQ(compilerHwInfoConfig->getCachingPolicyOptions(true), nullptr);
}

HWTEST2_F(CompilerHwInfoConfigFixture, givenAtLeastXeHpgCoreWhenGetCachingPolicyOptionsThenReturnWriteByPassPolicyOption, IsAtLeastXeHpgCore) {
    auto compilerHwInfoConfig = CompilerHwInfoConfig::get(defaultHwInfo->platform.eProductFamily);
    const char *expectedStr = "-cl-store-cache-default=2 -cl-load-cache-default=4";
    EXPECT_EQ(0, memcmp(compilerHwInfoConfig->getCachingPolicyOptions(false), expectedStr, strlen(expectedStr)));
    EXPECT_EQ(0, memcmp(compilerHwInfoConfig->getCachingPolicyOptions(true), expectedStr, strlen(expectedStr)));
}

HWTEST2_F(CompilerHwInfoConfigFixture, givenAtLeastXeHpgCoreWhenGetCachingPolicyOptionsThenReturnWriteBackPolicyOption, IsAtLeastXeHpgCore) {
    DebugManagerStateRestore restorer;
    DebugManager.flags.OverrideL1CachePolicyInSurfaceStateAndStateless.set(2);

    auto compilerHwInfoConfig = CompilerHwInfoConfig::get(defaultHwInfo->platform.eProductFamily);
    const char *expectedStr = "-cl-store-cache-default=7 -cl-load-cache-default=4";
    EXPECT_EQ(0, memcmp(compilerHwInfoConfig->getCachingPolicyOptions(false), expectedStr, strlen(expectedStr)));
    EXPECT_EQ(0, memcmp(compilerHwInfoConfig->getCachingPolicyOptions(true), expectedStr, strlen(expectedStr)));
}

HWTEST2_F(CompilerHwInfoConfigFixture, givenAtLeastXeHpgCoreAndDebugFlagSetForceAllResourcesUncachedWhenGetCachingPolicyOptionsThenReturnUncachedPolicyOption, IsAtLeastXeHpgCore) {
    DebugManagerStateRestore restorer;
    DebugManager.flags.OverrideL1CachePolicyInSurfaceStateAndStateless.set(2);
    DebugManager.flags.ForceAllResourcesUncached.set(true);

    auto compilerHwInfoConfig = CompilerHwInfoConfig::get(defaultHwInfo->platform.eProductFamily);
    const char *expectedStr = "-cl-store-cache-default=1 -cl-load-cache-default=1";
    EXPECT_EQ(0, memcmp(compilerHwInfoConfig->getCachingPolicyOptions(false), expectedStr, strlen(expectedStr)));
    EXPECT_EQ(0, memcmp(compilerHwInfoConfig->getCachingPolicyOptions(true), expectedStr, strlen(expectedStr)));
}

HWTEST2_F(CompilerHwInfoConfigFixture, givenCachePolicyWithoutCorrespondingBuildOptionWhenGetCachingPolicyOptionsThenReturnNullptr, IsAtLeastXeHpgCore) {
    DebugManagerStateRestore restorer;
    DebugManager.flags.OverrideL1CachePolicyInSurfaceStateAndStateless.set(5);

    auto compilerHwInfoConfig = CompilerHwInfoConfig::get(defaultHwInfo->platform.eProductFamily);
    EXPECT_EQ(nullptr, compilerHwInfoConfig->getCachingPolicyOptions(false));
    EXPECT_EQ(nullptr, compilerHwInfoConfig->getCachingPolicyOptions(true));
}
