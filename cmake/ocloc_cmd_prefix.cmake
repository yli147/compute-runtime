#
# Copyright (C) 2021-2023 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

if(WIN32)
  set(ocloc_cmd_prefix ocloc)
else()
  if(DEFINED NEO__IGC_LIBRARY_PATH)
	  set(ocloc_cmd_prefix ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${NEO__IGC_LIBRARY_PATH}:$<TARGET_FILE_DIR:ocloc_lib>" $<TARGET_FILE:ocloc>)
  else()
	  set(ocloc_cmd_prefix ${CMAKE_COMMAND} -E env "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:$<TARGET_FILE_DIR:ocloc_lib>" $<TARGET_FILE:ocloc>)
  endif()
endif()
