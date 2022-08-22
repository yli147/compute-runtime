/*
 * Copyright (C) 2018-2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "shared/source/commands/bxml_generator_glue.h"
#include "shared/source/gen9/hw_info.h"
#include "shared/source/helpers/debug_helpers.h"

#include "igfxfmid.h"

#include <cstddef>

template <class T>
struct CmdParse;

namespace NEO {
class LogicalStateHelper;
struct Gen9 {
#include "shared/source/generated/gen9/hw_cmds_generated_gen9.inl"

    static constexpr bool supportsSampler = true;
    static constexpr bool isUsingGenericMediaStateClear = true;
    static constexpr bool isUsingMiMemFence = false;

    struct FrontEndStateSupport {
        static constexpr bool scratchSize = true;
        static constexpr bool privateScratchSize = false;
    };

    struct StateComputeModeStateSupport {
        static constexpr bool threadArbitration = true;
        static constexpr bool coherency = false;
        static constexpr bool largeGrf = false;
        static constexpr bool zPassAsyncCompute = false;
        static constexpr bool pixelAsyncCompute = false;
        static constexpr bool preemptionMode = false;
    };

    struct StateBaseAddressStateSupport {
        static constexpr bool globalAtomics = false;
        static constexpr bool statelessMocs = true;
    };

    struct PipelineSelectStateSupport {
        static constexpr bool commandSupport = true;
    };

    struct PreemptionDebugSupport {
        static constexpr bool preemptionMode = true;
        static constexpr bool stateSip = true;
        static constexpr bool csrSurface = true;
    };

    struct DataPortBindlessSurfaceExtendedMessageDescriptor {
        union {
            struct {
                uint32_t bindlessSurfaceOffset : 20;
                uint32_t reserved : 1;
                uint32_t executionUnitExtendedMessageDescriptorDefinition : 11;
            };
            uint32_t packed;
        };

        DataPortBindlessSurfaceExtendedMessageDescriptor() {
            packed = 0;
        }

        void setBindlessSurfaceOffset(uint32_t offsetInBindlessSurfaceHeapInBytes) {
            bindlessSurfaceOffset = offsetInBindlessSurfaceHeapInBytes >> 6;
        }

        uint32_t getBindlessSurfaceOffsetToPatch() {
            return bindlessSurfaceOffset << 12;
        }
    };

    static_assert(sizeof(DataPortBindlessSurfaceExtendedMessageDescriptor) == sizeof(DataPortBindlessSurfaceExtendedMessageDescriptor::packed), "");
};

struct Gen9Family : public Gen9 {
    using PARSE = CmdParse<Gen9Family>;
    using GfxFamily = Gen9Family;
    using WALKER_TYPE = GPGPU_WALKER;
    using VFE_STATE_TYPE = MEDIA_VFE_STATE;
    using XY_BLOCK_COPY_BLT = typename GfxFamily::XY_SRC_COPY_BLT;
    using XY_COPY_BLT = typename GfxFamily::XY_SRC_COPY_BLT;
    using MI_STORE_REGISTER_MEM_CMD = typename GfxFamily::MI_STORE_REGISTER_MEM;
    using TimestampPacketType = uint32_t;
    using LogicalStateHelperHw = LogicalStateHelper;
    static const GPGPU_WALKER cmdInitGpgpuWalker;
    static const INTERFACE_DESCRIPTOR_DATA cmdInitInterfaceDescriptorData;
    static const MEDIA_INTERFACE_DESCRIPTOR_LOAD cmdInitMediaInterfaceDescriptorLoad;
    static const MEDIA_STATE_FLUSH cmdInitMediaStateFlush;
    static const MI_BATCH_BUFFER_END cmdInitBatchBufferEnd;
    static const MI_BATCH_BUFFER_START cmdInitBatchBufferStart;
    static const PIPE_CONTROL cmdInitPipeControl;
    static const MI_SEMAPHORE_WAIT cmdInitMiSemaphoreWait;
    static const RENDER_SURFACE_STATE cmdInitRenderSurfaceState;
    static const MI_LOAD_REGISTER_IMM cmdInitLoadRegisterImm;
    static const MI_LOAD_REGISTER_REG cmdInitLoadRegisterReg;
    static const MI_LOAD_REGISTER_MEM cmdInitLoadRegisterMem;
    static const MI_STORE_DATA_IMM cmdInitStoreDataImm;
    static const MI_STORE_REGISTER_MEM cmdInitStoreRegisterMem;
    static const MI_NOOP cmdInitNoop;
    static const MI_REPORT_PERF_COUNT cmdInitReportPerfCount;
    static const MI_ATOMIC cmdInitAtomic;
    static const PIPELINE_SELECT cmdInitPipelineSelect;
    static const MI_ARB_CHECK cmdInitArbCheck;
    static const MEDIA_VFE_STATE cmdInitMediaVfeState;
    static const STATE_BASE_ADDRESS cmdInitStateBaseAddress;
    static const MEDIA_SURFACE_STATE cmdInitMediaSurfaceState;
    static const SAMPLER_STATE cmdInitSamplerState;
    static const GPGPU_CSR_BASE_ADDRESS cmdInitGpgpuCsrBaseAddress;
    static const STATE_SIP cmdInitStateSip;
    static const BINDING_TABLE_STATE cmdInitBindingTableState;
    static const MI_USER_INTERRUPT cmdInitUserInterrupt;
    static const XY_BLOCK_COPY_BLT cmdInitXyBlockCopyBlt;
    static const XY_SRC_COPY_BLT cmdInitXyCopyBlt;
    static const MI_FLUSH_DW cmdInitMiFlushDw;
    static const XY_COLOR_BLT cmdInitXyColorBlt;

    static constexpr bool supportsCmdSet(GFXCORE_FAMILY cmdSetBaseFamily) {
        return cmdSetBaseFamily == IGFX_GEN8_CORE;
    }
};

} // namespace NEO
